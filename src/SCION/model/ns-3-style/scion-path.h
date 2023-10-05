#pragma once
#include <stdint.h>
#include "ns3/buffer.h"
#include <memory>
#include "scion-path-forward.h"
#include "ns3/iterator-util.h"
#include "ns3/scion-utils.h"
#include <optional>
#include "ns3/go-errors.h"
#include <expected>

namespace ns3
{


/*


// Metadata defines a new SCION path type, used for dynamic SICON path type registration.
struct Metadata  {
	// Type is a unique value for the path.
	pathType_t Type;
	// Desc is the description/name of the path.
	std::string Desc;
	// New is a path constructor function.
	 Path New();
};


struct metadata  {
	bool inUse ;
	Metadata metaData;
};


	[maxPathType]metadata registeredPaths ;
	bool strictDecoding  = true;
)



func (t Type) String() string {
	pm := registeredPaths[t]
	if !pm.inUse {
		return fmt.Sprintf("UNKNOWN (%d)", t)
	}
	return fmt.Sprintf("%v (%d)", pm.Desc, t)
}

*/


// Path is the path contained in the SCION header.


class Path
{
    class PathConcept
    {
    public:
	// SerializeTo serializes the path into the provided buffer.
	virtual void Serialize( buffer_iterator start ) const = 0;
	// DecodesFromBytes decodes the path from the provided buffer.
	virtual std::expected<uint32_t,error> Deserialize( const_buffer_iterator start ) =0;

	/*
	virtual void Serialize( Buffer::Iterator start ) const = 0;	
	virtual uint32_t Deserialize( Buffer::ConstIterator start ) = 0;
	*/
	// Reverse reverses a path such that it can be used in the reversed direction.
	//
	// XXX(shitz): This method should possibly be moved to a higher-level path manipulation package.
	//virtual Path& Reverse() = 0; this was impractical 
	virtual void Reverse() =0;

	// Len returns the length of a path in bytes.
	virtual uint16_t Len() const = 0 ;
	// Type returns the type of a path.
	virtual pathType_t Type() const= 0 ;
    virtual ~PathConcept() {}

	

	template<typename T>
	std::optional<T*> As()
	{	if(T::Type() == Type() )
		{return reinterpret_cast<T*>( get_runtime_path() ); 
		}
		else
		{
			return std::nullopt;
		}
	}
	protected:
	virtual void* get_runtime_path() = 0;
};


   template< class T >
    struct PathModel : PathConcept
     {
    //   PathModel(  T&& t ) : object( std::forward<T>(t) ) {}

    explicit PathModel(T& value) :       object(_NonConstReference(value)){}
   // explicit PathModel( T const& value) : object(_ConstReference(value)){} // makes little sense here ^^ remove later
    explicit PathModel( T&& value) :      object(_Value(std::move(value))){}


       virtual ~PathModel() {}

	virtual void Serialize( buffer_iterator start )const
	{
		static_cast<const T&>( object).Serialize( start );
	}

	virtual std::expected<uint32_t,error> Deserialize( const_buffer_iterator start )
	{
	return static_cast<T&>(object).Deserialize(start);
	}

	/*
    virtual void Serialize( Buffer::Iterator start ) const 
    {  static_cast<T&>(object).Serialize(start);
	}
	
	virtual uint32_t Deserialize( Buffer::ConstIterator start )
    { return static_cast<T&>(object).Deserialize( start );}
	*/
	
	//virtual Path& Reverse()
	virtual void Reverse()
	{ 
		static_cast<T&>(object).Reverse();
		//return getPath();

		//return object.Reverse();  // this wont work ^^
	}
	
	virtual uint16_t Len() const { return static_cast<T&>(object).Len(); }
	
	virtual pathType_t Type()const { return static_cast<T&>(object).Type(); }
       
     private:
	virtual void* get_runtime_path() override
	{ return &static_cast<T&>(object); 
	}

	/* // no longer needed after refactoring of Reverse()
	Path& getPath()
	 {
		static Path _path;
		_path = Path( static_cast<T&>(object) );

		return _path;
	 }*/

     mutable Storage< T > object;
   };

   std::shared_ptr< PathConcept> object = nullptr;

  public:
	Path() = default; // Path has to be default constructible,
	// in order for higher level objects such as SCIONHeader who contain a Path to be default constructible

   template< typename T > 
   Path(  T& obj ) :   object( std::make_shared< PathModel<T>>( obj ) )  {}

   template< typename T > 
   Path(  T&& obj ) :   object( std::make_shared< PathModel<T>>( std::move(obj) ) )  {}


   void Serialize( buffer_iterator start )const
   {
	if(object)
	{
		object->Serialize(start);
	}
   }

   std::expected<uint32_t,error> Deserialize( const_buffer_iterator start )
   {
	if(object)
	{
		return object->Deserialize(start);
	}
	return std::unexpected( proto_error{"cannot deserialize default constructed empty path"} );
   }

	/*
	// TODO: maybe add error parameter as in Go
    void Serialize( Buffer::Iterator start ) const 
    { if(object ) // null if default constructed
		object->Serialize(start);
	}
	
	// TODO: maybe add error parameter as in Go
	uint32_t Deserialize( Buffer::ConstIterator start )
    {
		if(object)
		{return object->Deserialize(start);}
		else 
		{return 0;}
	}
	*/

	// TODO: maybe add error parameter as in Go
	Path& Reverse()
    {
		if(object)
		{ //return object->Reverse();// this wont work ^^
		object->Reverse();
		return *this;
		}
		else{return *this;}
	} 
	
	uint16_t Len() const 
	{
		if(object)
		{ return object->Len(); }
		else
		{ return 0;}// this should be EmptyPathType
	}
	
	pathType_t Type() const
	{	
		if( object )
		{ return object->Type(); 
		}
		return pathType_t::EmptyPath;
	}

	/*
	why not 'IncPath()' , "GetHopField()" IsFstHopAfterXover()
	GetCurrentInfoField()
	 and etc. here ???
	*/
};

/*


// RegisterPath registers a new SCION path type globally.
// The PathType passed in must be unique, or a runtime panic will occur.
func RegisterPath(pathMeta Metadata) {
	pm := registeredPaths[pathMeta.Type]
	if pm.inUse {
		panic("path type already registered")
	}
	registeredPaths[pathMeta.Type].inUse = true
	registeredPaths[pathMeta.Type].Metadata = pathMeta
}

// StrictDecoding enables or disables strict path decoding. If enabled, unknown
// path types fail to decode. If disabled, unknown path types are decoded into a
// raw path that keeps the encoded path around for re-serialization.
//
// Strict parsing is enabled by default.
//
// Experimental: This function is experimental and might be subject to change.
func StrictDecoding(strict bool) {
	strictDecoding = strict
}

// NewPath returns a new path object of pathType.
func NewPath(pathType Type) (Path, error) {
	pm := registeredPaths[pathType]
	if !pm.inUse {
		if strictDecoding {
			return nil, serrors.New("unsupported path", "type", pathType)
		}
		return &rawPath{}, nil
	}
	return pm.New(), nil
}





// NewRawPath returns a new raw path that can hold any path type.
func NewRawPath() Path {
	return &rawPath{}
}

type rawPath struct {
	raw      []byte
	pathType Type
}

func (p *rawPath) SerializeTo(b []byte) error {
	copy(b, p.raw)
	return nil
}

func (p *rawPath) DecodeFromBytes(b []byte) error {
	p.raw = b
	return nil
}

func (p *rawPath) Reverse() (Path, error) {
	return nil, serrors.New("not supported")
}

func (p *rawPath) Len() int {
	return len(p.raw)
}

func (p *rawPath) Type() Type {
	return p.pathType
}
*/

} // ns3 namespace 
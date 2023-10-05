#pragma once

#include "ns3/scion-types.h"
//#include "ns3/scion-path.h"
#include "ns3/scion-snet-path.h"
#include "ns3/scion-address.h"
#include "ns3/scion-utils.h"

#include <vector>

namespace ns3
{




class SCIONPathSelector
{   

// Selector controls the path used by a single **dialed** socket. Stateful.
class SCIONPathSelectorConcept
{
public:
	// Path selects the path for the next packet.
	// Invoked for each packet sent with Write.
virtual const SNETPath& Path()const = 0;
// Initialize the selector for a connection with the initial list of paths,
	// filtered/ordered by the Policy.
	// Invoked once during the creation of a Conn.

virtual void Initialize( const SCIONAddress& local,
                const SCIONAddress& remote,
				 std::vector< SNETPath > paths ) = 0;

// Refresh updates the paths. This is called whenever the Policy is changed or
	// when paths were about to expire and are refreshed from the SCION daemon.
	// The set and order of paths may differ from previous invocations.
virtual void Refresh( std::vector<SNETPath>&) = 0;

// PathDown is called whenever an SCMP down notification is received on any
	// connection so that the selector can adapt its path choice. The down
	// notification may be for unrelated paths not used by this selector.
virtual void PathDown( PathHash_t, PathInterface ) = 0;

virtual void Close() = 0;

};

   template< class T >
    struct PathSelectorModel : SCIONPathSelectorConcept
     {
    //   PathModel(  T&& t ) : object( std::forward<T>(t) ) {}

    explicit PathSelectorModel(T& value) :       object(_NonConstReference(value)){}
   // explicit PathSelectorModel( T const& value) : object(_ConstReference(value)){} // makes little sense here ^^ remove later
    explicit PathSelectorModel( T&& value) :      object(_Value(std::move(value))){}


       virtual ~PathSelectorModel() {}

    virtual const SNETPath& Path() const 
    {  return static_cast<const T&>(object).Path();
	}
	
	virtual void Initialize( const SCIONAddress& local,
                const SCIONAddress& remote,
				 std::vector< SNETPath > paths )
    {  static_cast<T&>(object).Initialize(local,remote,paths); }
	
	virtual void Refresh( std::vector<SNETPath>& p)
	{ 
		static_cast<T&>(object).Refresh(p);
		
	}
	
	virtual void PathDown( PathHash_t hash, PathInterface pi) 
	 {  static_cast<T&>(object).PathDown(hash,pi); }
	
	virtual void Close() 
	{ static_cast<T&>(object).Close(); }
       
    
     Storage< T > object;
   };

   std::shared_ptr< SCIONPathSelectorConcept > object = nullptr;

  public:
	SCIONPathSelector() = default; // Path has to be default constructible,
	// in order for higher level objects such as SCIONHeader who contain a Path to be default constructible

   template< typename T > 
   SCIONPathSelector(  T& obj ) :   object( std::make_shared< PathSelectorModel<T>>( obj ) )  {}

   template< typename T > 
   SCIONPathSelector(  T&& obj ) :   object( std::make_shared< PathSelectorModel<T>>( std::move(obj) ) )  {}

    const SNETPath& Path(  ) const 
    { //if(object ) // null if default constructed
		return object->Path();
	}
	
	void Initialize( const SCIONAddress& local,
                const SCIONAddress& remote,
				 std::vector< SNETPath > paths )
    {
		if(object)
		{ object->Initialize(local,remote,paths);
		}
		
	}
	
	void Refresh( std::vector<SNETPath>& p)
    {
		if(object)
		{  object->Refresh( p );
		}
	} 
	
	void PathDown( PathHash_t hash, PathInterface pi) 
	{
		if(object)
		{  object->PathDown(hash,pi); 
		}
		
	}
	
	void Close()
	{	
		if( object )
		{  object->Close(); 
		}
	}
};




}
#pragma once
#include "ns3/buffer.h"
#include "ns3/packet.h"
#include "ns3/iterator-util.h"

#include <functional>
#include <map>
#include <memory>
#include <compare>
#include <ranges>
#include <optional>
#include <stdint.h>
#include <algorithm>
#include <expected>
#include "ns3/go-errors.h"

#include "neo/bytes.hpp"

namespace ns3
{

struct LayerType;
struct LayerBase;
struct LayerStore;

using input_t = const_buffer_iterator;
using output_t = buffer_iterator;


using Decoder_t = std::function< std::expected<input_t,error> (input_t, LayerStore*)>;

/*
struct bufferIterRangeAdaptor_t
{

bufferIterRangeAdaptor_t(Buffer::Iterator start )
: m_start(start)
{
}

Buffer::Iterator m_start;

auto begin()
{
return m_start;
}

auto end()
{
return m_start + m_start.GetSize();
}

};

// TODO properly typeErase the decoder
struct decoder_t
{
    decoder_t( std::function<void(Buffer::Iterator,LayerStore*)> bufferDecoder )
    : m_dec( [ buffDec =std::move(bufferDecoder) ]()
            { return [](bytes b,LayerStore* store)
                        { buffDec(,); };
            }
    )
    {}

    void opertator()(Buffer::Iterator, LayerStore* );

    void operator()(bytes, LayerStore*);
private:
    std::function< byteDecoder()> m_dec;

};
*/

struct LayerClass
{
    LayerClass(const std::vector<LayerType>& vec ) : m_vec(vec){}
    bool contains(const LayerType&lt) const 
    { return std::find(m_vec.begin(), m_vec.end(), lt) != std::end(m_vec); } /*return std::ranges::contains( m_vec,lt);*/ 
    const std::vector<LayerType>& Types() const {return m_vec;}
private:
    std::vector<LayerType> m_vec;
};

struct LayerType 
{
    LayerType(uint32_t type)
        : m_type(type)
    {
    }

    uint32_t m_type;

    // Decode decodes the given data using the decoder registered with the layer type.
    std::expected<input_t,error> Decode(input_t data, LayerStore* c) const;

    // or return std::pair< input_t, LayerType*>

    // MAYBE the signature should change to LayerType* Decode( input_t data, LayerStore* )
    // and return the Next(Layer)Type as a result

    auto operator==(const LayerType& o) const
    {
        return m_type == o.m_type;
    }
};


// SerializeOptions provides options for behaviors that SerializableLayers may want to
// implement.
struct SerializeOptions
{
    // FixLengths determines whether, during serialization, layers should fix
    // the values for any length field that depends on the payload.
    bool FixLengths = 1;
    // ComputeChecksums determines whether, during serialization, layers
    // should recompute checksums based on their payloads.
    bool ComputeChecksums = 1;
};

// LayerStore combines DecodingFeedback |DecodeOptions| PacketBuilder | SerializeOptions aspects of packets into one base class
// 
// TODO: add DecodingFeedback parameter to Deserialize()
struct LayerStore
{

    const SerializeOptions& serialize_options() const { return m_opts;} // this is actually from SerializeBuffer which is implemented by packet

    virtual void AddLayer(std::shared_ptr<LayerBase>) = 0; // called by the Decoder if its done

    //  virtual void NextDecoder(Decoder_t) = 0;
    // if i wanted to do that, i had to typeErase (Decoder is an interface in go)

    // virtual void NextDecoder(LayerType*) = 0; // since any LayerType is a Decoder
    virtual std::expected<input_t,error> NextDecoder(const LayerType&) = 0;
     // called by the Decoders if 'decode_recursive' is true


    // maybe provide overload void NextDecoder( layerType* , Buffer::Iterator read_from_here )

    // this signature could also change to return the Next(Layer)Type as a pointer (if any)
    bool decode_recursive( )const {return m_decode_recursive;}  // called by Decoders such as i.e. LayerType
   void set_decode_recursive( bool rec ){ m_decode_recursive = rec; }

    void set_truncated() {m_truncated = true;}
    bool is_truncated()const{return m_truncated;}

protected:
   // void set_decode_recursive( bool r) { m_decode_recursive = r; }    // called by the packet(no, its not a direct base class)
    private:
    bool m_decode_recursive = false;
    SerializeOptions m_opts;

    // ---- DecodingFeedback -----------
    bool m_truncated=false;
};

struct LayerTypeMetaData
{
    std::string m_name;
    Decoder_t m_decoder;
};

struct LayerTypeRegistry
{
    static LayerTypeRegistry& instance();
    LayerType RegisterLayerType(LayerTypeMetaData meta);

  private:

    LayerType GetByName( const std::string & layerTypeName ) const;
    friend struct LayerType;

    // DecodersByLayerName maps layer names to decoders for those layers.
    // This allows users to specify decoders by name to a program and have that
    // program pick the correct decoder accordingly.
    std::map<std::string, Decoder_t> decodersByLayerName;

    static constexpr uint32_t maxLayerType = 2000;

    // var ltMeta [maxLayerType]layerTypeMetadata

    std::map<uint32_t, LayerTypeMetaData> layer_type_meta_map;
};

struct LayerBase
{
    virtual LayerType Type() const = 0;
    virtual input_t Contents() const = 0;
    virtual input_t Payload() const = 0;
};

// maybe this should be renamed to DeserializingLayer to be consistent ^^
struct DecodingLayer : virtual public LayerBase
{
    virtual std::optional< LayerType > NextType() const = 0;
    virtual LayerClass CanDecode() const = 0;
    virtual input_t Payload() const = 0;

    // here contents and payload pointers are set
    virtual std::expected< uint32_t,error> Deserialize( input_t, LayerStore* = nullptr ) = 0;
    // should this be input_t here ?!
};



struct SerializableLayer : virtual public LayerBase
{
    // for the legacy Packet with Add/Remove Header()
    virtual void Serialize(Buffer::Iterator) const = 0;

     
    virtual output_t Serialize(  output_t start , SerializeOptions ) const = 0;
    virtual uint32_t GetSerializedSize() const =0;

    // LayerType returns the type of the layer that is being serialized to the buffer
    // virtual LayerType* Type() const = 0;
};

/*
// SerializeBuffer was unneccessary !!
struct SerializeBuffer
{

    // Clear resets the SerializeBuffer to a new, empty buffer.  After a call to clear,
    // the byte slice returned by any previous call to Bytes() for this buffer
    // should be considered invalidated.
    void Clear();

    //  I DONT SEE A REASON FOR THEESE  -> they where never called anywhere
    // Layers returns all the Layers that have been successfully serialized into this buffer
    // already.
    std::vector<std::shared_ptr< LayerBase> > Layers();
    // PushLayer adds the current Layer to the list of Layers that have been serialized
    // into this buffer.
    void PushLayer(LayerType);

    std::vector<std::shared_ptr< LayerBase> > layers;


    bytes data;
    int start;
    int prepended;
    int appended;
};*/

struct Layer : public virtual  DecodingLayer,public virtual SerializableLayer
{
};

// LayerStore or PacketBuilder is for Decoding
// maybe also inherit SerializationBuffer for Serialization
// no thats rubbish. This is completely covered by the Base Class Packet's Buffer
class GoPacket : public SimpleRefCount<GoPacket, Packet>,public LayerStore
{
  public:
  GoPacket(){}

GoPacket( const Packet& p)
{
    static_cast< Packet*>( static_cast< SimpleRefCount<GoPacket,Packet>* >(this) )->operator=(  static_cast< const Packet&>( static_cast<const SimpleRefCount<GoPacket,Packet>& >(p) )   );
}

    virtual ~GoPacket()
    {
    }

    void Serialize() const;

    uint32_t GetSerializedSize()const;

   
    error InitialDecode( const LayerType& dec);

    error Decode(const LayerType& dec);



    std::vector<std::shared_ptr<LayerBase>> Layers()
    {
        return m_layers;
    }

    std::shared_ptr<LayerBase> Layer(LayerType);

    // void AddHeader(const Header& header, SerializationOptions opts); // overloads of Packet
    // methods

    // change signature to return error if i.e. newLayer is non serializable or decodable
    virtual void AddLayer(std::shared_ptr<LayerBase> newLayer)
    {
        m_layers.push_back(newLayer);
    }

    template< class Layer>
    void AddLayer( Layer&& l)
    {
        m_layers.push_back(  std::make_shared<Layer>( std::forward<Layer>(l) )  );
    }

  private:
 

  friend class LayerType; // in order for the decoder to be able to call the LayerStore interface methods

    // decode packet eager
    std::expected<input_t,error> NextDecoder( const LayerType& dec) override;



    std::vector<std::shared_ptr<LayerBase>> m_layers;
  //  bytes m_data;
};

std::expected<input_t,error> decodePayload( input_t , LayerStore*);


struct Payload_t : virtual public Layer, public Header
{
    static LayerType staticLayerType(){ return m_type;}

    LayerType Type() const override  {     return m_type;   }

    // Contents returns the bytes making up this layer.
    input_t Contents() const 
    {
        return m_start;
    }

    input_t Payload() const 
    {
        return m_start + m_start.GetRemainingSize();
    } // an invalid end-iterator

    LayerClass CanDecode() const override
    {
        return LayerClass{ {staticLayerType()} };
    }

    std::optional<LayerType> NextType() const override
    {
        return std::nullopt;
    }

    uint32_t Deserialize(Buffer::Iterator start) override
    {
        auto tmp= Deserialize( input_t( start ),nullptr );
        if( tmp )
        {
            return *tmp;
        }
        else
        {
          throw error_exception( tmp.error() );
        }
    }

    // Payload cannot be truncated, so the LayerStore parameter is  unused 
    std::expected<uint32_t,error> Deserialize(input_t start, LayerStore* )
    {
        m_start = start;
        m_bytes = neo::bytes::copy( neo::as_buffer( &(*m_start), m_start.GetRemainingSize()) );

        return m_start.GetRemainingSize();
    }

    void Serialize( Buffer::Iterator ) const override;
    output_t Serialize( output_t start , SerializeOptions ) const ;
    uint32_t GetSerializedSize() const ;
     static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;

    void Print(std::ostream& os) const override;


  neo::bytes m_bytes;
private:

    inline static LayerType m_type = LayerTypeRegistry::instance().RegisterLayerType(LayerTypeMetaData{
                                .m_name = "Payload",
                                .m_decoder = &decodePayload                                });


    input_t m_start;
  
};

} // namespace ns3

template <>
struct std::hash<ns3::LayerType>
{
    std::size_t operator()(const ns3::LayerType& lt) const
    {
        return lt.m_type;
    }
};
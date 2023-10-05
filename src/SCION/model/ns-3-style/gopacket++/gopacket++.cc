#include "ns3/gopacket++.h"

#include "neo/buffer_algorithm/copy.hpp"
#include "neo/bytewise_iterator.hpp"
#include <algorithm>
#include "ns3/basic-error.h"

#include <ranges>
namespace ns3
{

 // parse the initial outermost Layer
//  trying to decode a layer, which is not present at the packet beginning will fail
 // initialDecode could return something that tells us, how far/deep it recursed into the packet 
error GoPacket::InitialDecode(const LayerType& dec )
{
   [[maybe_unused]] auto end_of_layer = dec.Decode(GetBuffer().CBegin(), this);
   if( end_of_layer)
   {
    return {}; // return nil-error to indicate success
   } else
   {
    return end_of_layer.error(); // otherwise return, what went wrong
   }
}


    // actually unnecessary, the way decodeSCION is implemented now (always recursive decoding)
    //  No need to return anything ( the existence of any iterators into the buffer are internals of the packet)
    // the enduser should see Layers as lowest level of granularity
error GoPacket::Decode( const LayerType& dec )
{

     if( auto res =   InitialDecode(dec); res )
     {
        return res;
     }

        if( auto next_type = std::dynamic_pointer_cast<DecodingLayer>(m_layers.back())->NextType(); next_type.has_value() )
        {
            [[maybe_unused]] auto iter_end = NextDecoder( next_type.value() );
            if( !iter_end )
            {
                return iter_end.error();
            }
        }
    return {}; // nil error indicates success
}


std::expected<input_t, error> GoPacket::NextDecoder(const LayerType& dec )
{

        
        if( m_layers.size() == 0 )
         {         //    "cannot call NextDecoder with empty Layers. Did you forget AddLayers() ?!";
         return std::unexpected( basic_error{ "NextDecoder called, but no layers added yet" } );
         }

        auto d = m_layers.back()->Payload();

       // input_t end_of_layer;

        // packet completely decoded ?! or still payload left left
        if (d.size() != 0)
        {
            // Since we're eager, immediately call the next decoder.
            auto tmp = dec.Decode(d, this);
           
            return tmp;
        }

        NS_ASSERT_MSG(d == d.cend(), "Logic error in iterator impl" );
        return d; // go simply returns nil-error here
                  // the c++ version returns an invalid end-iterator
}

uint32_t GoPacket::GetSerializedSize()const
{

        uint32_t total_size=0;
        //auto & buffer = const_cast<GoPacket&>(*this).GetBuffer();
      
        for ( const auto& layer : m_layers )
        {
            auto serializable_layer = std::dynamic_pointer_cast<const SerializableLayer>( layer );
            NS_ASSERT_MSG(serializable_layer, "You must not try to store non-serializable layers into your GoPacket");
            total_size+= serializable_layer->GetSerializedSize();
        }
        return total_size;  
}

void GoPacket::Serialize() const
{

        auto & buffer = const_cast<GoPacket&>(*this).GetBuffer();
        auto & byteTagList = const_cast<GoPacket&>(*this).m_byteTagList;

         // buffer.AddAtEnd( GetSerializedSize() );
//        buffer = Buffer( GetSerializedSize(),false );
        auto size = GetSerializedSize();
        buffer.AddAtStart(size);
        byteTagList.Adjust(size);
        byteTagList.AddAtStart(size);

        buffer_iterator start = buffer.Begin();
        auto start_cpy{start};
        for ( const auto& layer : m_layers )
        {
            auto serializable_layer = std::dynamic_pointer_cast<const SerializableLayer>( layer );
            NS_ASSERT_MSG(serializable_layer, "You must not try to store non-serializable layers into your GoPacket");
          
            start = serializable_layer->Serialize(start,serialize_options() );
        }
        NS_ASSERT_MSG( start_cpy.distance_to(start) == size ,"wrong number of bytes written in packet serialize()" );
}

 TypeId Payload_t::GetTypeId()
 {
     static TypeId tid = TypeId("ns3::Payload_t")
                            .SetParent<Header>()
                            .SetGroupName("Internet")
                            .AddConstructor<Payload_t>();
    return tid;
 }
    
    TypeId Payload_t::GetInstanceTypeId() const
    {
        return GetTypeId();
    }

    void Payload_t::Print( [[maybe_unused]] std::ostream& os) const
    {

    }

    void Payload_t::Serialize( [[maybe_unused]] Buffer::Iterator  start ) const
    {     

        auto buff = neo::const_buffer(m_bytes.data(),m_bytes.size());
        auto iter = neo::bytewise_iterator(buff);

        std::ranges::copy( iter.begin(),iter.end() ,start ) ; 
      
    }

    output_t Payload_t::Serialize( [[maybe_unused]] output_t start, [[maybe_unused]] SerializeOptions opts ) const 
    {
       
        auto buff = neo::const_buffer(m_bytes.data(),m_bytes.size());
        auto iter = neo::bytewise_iterator(buff);

    std::ranges::copy( iter.begin(),iter.end() ,start ) ; //neo::as_buffer( m_bytes).begin() 
    return start + GetSerializedSize();
    }

    uint32_t Payload_t::GetSerializedSize() const 
    {
      //  return m_start.GetRemainingSize();
      return m_bytes.size();
    }

// decodePayload decodes data by returning it all in a Payload layer.
std::expected<input_t,error> decodePayload( input_t start, LayerStore* store)
{
    std::shared_ptr<LayerBase > payload = std::make_shared<Payload_t> ();
    auto payloadlayer = std::dynamic_pointer_cast<Payload_t>(payload);

    uint32_t bytes;
   
   if(auto bytesread = payloadlayer->Deserialize( start,store ); bytesread )
   {
    bytes = *bytesread;
   }else
   {
    return std::unexpected( bytesread.error() );
   }

    store->AddLayer( payload );
    
    return start + bytes;
}


std::shared_ptr< LayerBase> GoPacket::Layer( LayerType type)
{
    for( const auto& layer : m_layers )
    {
        if( layer->Type().m_type == type.m_type )
        {
            return  layer;
        }
    }
    return nullptr;
}


LayerType LayerTypeRegistry::GetByName( const std::string & layerTypeName ) const
{
    for( const auto& meta : layer_type_meta_map )
    {
        if( meta.second.m_name == layerTypeName )
        { 
            return LayerType{ meta.first };
        }
    }
    NS_ASSERT_MSG(false,"non existing LayerType: " << layerTypeName << " requested");
}

LayerTypeRegistry& LayerTypeRegistry::instance()
{
static LayerTypeRegistry reg;
return reg;
}

LayerType LayerTypeRegistry::RegisterLayerType( LayerTypeMetaData meta )
{
    if( decodersByLayerName.contains( meta.m_name) )
    {
        NS_ASSERT_MSG(false," cant register LayerType: " << meta.m_name << " twice!" );
    }
    
    auto m_type = decodersByLayerName.size() ;
    decodersByLayerName.emplace(  meta.m_name, meta.m_decoder );

    layer_type_meta_map.emplace( m_type, meta );

    return m_type;
}

// Decode decodes the given data using the decoder registered with the layer
// type.
std::expected<input_t,error> LayerType::Decode( input_t data , LayerStore* c)const
{
	Decoder_t d = LayerTypeRegistry::instance().layer_type_meta_map.at(m_type).m_decoder;
	
	//if d != nil {
		//return d.Decode(data, c);
     return   d(data,c);
	//}
	// return fmt.Errorf("Layer type %v has no associated decoder", t)
}
}
#pragma once

#include "ns3/gopacket++.h"
#include "ns3/udp-header.h"

namespace ns3
{


std::expected<input_t,error> decodeUDPLayer( input_t , LayerStore*);


struct UDPLayer : virtual public Layer,  virtual  public UdpHeader
{
    // returns 8x bytes
    uint32_t GetSerializedSize()const override{ return UdpHeader::GetSerializedSize(); }
    void Serialize( Buffer::Iterator it)const { UdpHeader::Serialize(it) ; }
    static LayerType staticLayerType(){ return m_type;}

    LayerType Type() const override  {     return m_type;   }

    // Contents returns the bytes making up this layer.
    input_t Contents() const    {      return m_start;   }

    input_t Payload() const {return m_start+UdpHeader::GetSerializedSize(); };
   

   // payload is automatically computed from the iterator::GetRemainingSize() on serialization
   // and does not have to be set manually
    uint16_t GetPayloadSize()const;

    LayerClass CanDecode() const override
    {
        return LayerClass{ {staticLayerType()} };
    }

    std::optional<LayerType> NextType() const override;

    std::expected<uint32_t,error> Deserialize(input_t start, LayerStore* = nullptr ); 
  
    output_t Serialize( output_t start , SerializeOptions ) const ;

    
     static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override{return GetTypeId(); }

    
private:

    inline static LayerType m_type = LayerTypeRegistry::instance().RegisterLayerType(LayerTypeMetaData{
                                .m_name = "UDPLayer",
                                .m_decoder = &decodeUDPLayer
                                });


    input_t m_start;
  
};

}
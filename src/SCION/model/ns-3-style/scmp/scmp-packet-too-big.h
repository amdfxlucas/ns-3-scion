#pragma once
#include "ns3/gopacket++.h"

namespace ns3
{
    std::expected<input_t,error> decodeSCMPPacketTooBig(input_t , LayerStore*);

    /*!
     \brief SCMPPacketTooBig represents the structure of a packet too big message.
    
    	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    	|            reserved           |             MTU               |
    	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */

    class SCMPPacketTooBig : virtual public Layer, public Header
    {
    public:
    
      SCMPPacketTooBig( uint16_t mtu = std::numeric_limits<uint16_t>::max() ) : m_mtu(mtu){}
      auto GetMtu()const{return m_mtu; }

  static LayerType staticLayerType(){ return m_type;}

    LayerType Type() const override  {     return m_type;   }

    // Contents returns the bytes making up this layer.
    input_t Contents() const override;
    input_t Payload() const override;
    LayerClass CanDecode() const override;
    std::optional<LayerType> NextType() const override;

    uint32_t Deserialize( Buffer::Iterator start) override;

    std::expected<uint32_t, error> Deserialize( input_t start, LayerStore* = nullptr );

    void Serialize( Buffer::Iterator ) const override;
    output_t Serialize( output_t start , SerializeOptions ) const ;
    uint32_t GetSerializedSize() const ;
     static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;

    void Print(std::ostream& os) const override;


  
private:

    inline static LayerType m_type = LayerTypeRegistry::instance().RegisterLayerType(LayerTypeMetaData{
                                .m_name = "SCMPPacketTooBig",
                                .m_decoder = &decodeSCMPPacketTooBig          });


    input_t m_start;
    input_t m_payload;

    
        uint16_t m_mtu;
    };

    

}
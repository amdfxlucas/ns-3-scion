#pragma once

#include "ns3/scion-types.h"
#include "ns3/gopacket++.h"

namespace ns3
{

    std::expected<input_t,error> decodeSCMPTraceroute( input_t, LayerStore* );
/*!
    \brief SCMPTraceroute represents the structure of a traceroute.

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|           Identifier          |        Sequence Number        |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|              ISD              |                               |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+         AS                    +
	|                                                               |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                                                               |
	+                        Interface ID                           +
	|                                                               |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

    class SCMPTraceroute : public virtual Layer, public Header
    {
        public:
             

  static LayerType staticLayerType(){ return m_type;}

    LayerType Type()const;

    // Contents returns the bytes making up this layer.
    input_t Contents() const override;
    input_t Payload() const override;
    LayerClass CanDecode() const override;

    std::optional<LayerType> NextType() const override;

    uint32_t Deserialize(Buffer::Iterator start) override;
    std::expected<uint32_t,error> Deserialize(input_t start, LayerStore* = nullptr);

    void Serialize( Buffer::Iterator start ) const override;
    output_t Serialize( output_t start , SerializeOptions ) const ;
    uint32_t GetSerializedSize() const ;
     static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;

    void Print(std::ostream& os) const override;


  
private:

    inline static LayerType m_type = LayerTypeRegistry::instance().RegisterLayerType(
                                LayerTypeMetaData{
                                .m_name = "SCMPTraceroute",
                                .m_decoder = &decodeSCMPTraceroute    });


    input_t m_start;
    input_t m_payload;
             

        uint16_t m_identifier;
        uint16_t m_sequence;
        IA_t m_ia;
        ASIFID_t m_interface;
    };

    

}
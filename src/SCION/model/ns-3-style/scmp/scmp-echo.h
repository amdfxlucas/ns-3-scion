#pragma once
#include "ns3/gopacket++.h"

namespace ns3
{

std::expected<input_t,error> decodeSCMPEcho(input_t, LayerStore*);

/*!
  \brief SCMPEcho represents the structure of a ping.

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|           Identifier          |        Sequence Number        |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
class SCMPEcho : public virtual Layer, public Header
{
public:
SCMPEcho(){}
SCMPEcho( uint16_t id, uint16_t seq )
: m_identifier(id),
 m_sequence_number(seq)
 {}

  static LayerType staticLayerType(){ return m_type;}

    LayerType Type() const override ;

    // Contents returns the bytes making up this layer.
    input_t Contents()const override;
    input_t Payload() const override;

    LayerClass CanDecode() const override;

    std::optional<LayerType> NextType() const override;

    uint32_t Deserialize(Buffer::Iterator start) override;

    std::expected<uint32_t,error> Deserialize(input_t start , LayerStore * = nullptr );

    void Serialize( Buffer::Iterator start ) const override;
    output_t Serialize( output_t start , SerializeOptions ) const ;
    uint32_t GetSerializedSize() const ;
     static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;

    void Print(std::ostream& os) const override;


  
private:

    inline static LayerType m_type = LayerTypeRegistry::instance().RegisterLayerType(
                                LayerTypeMetaData{
                                .m_name = "SCMPEcho",
                                .m_decoder = &decodeSCMPEcho });

input_t m_content;
input_t m_payload;

uint16_t m_identifier=0;
uint16_t m_sequence_number=0;
};



}
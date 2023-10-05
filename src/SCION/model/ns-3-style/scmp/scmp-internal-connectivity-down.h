#pragma once
#include "ns3/gopacket++.h"
#include "ns3/scion-types.h"

namespace ns3
{

std::expected<input_t,error> decodeSCMPInternalConnectivityDown(input_t , LayerStore* );

/*!
  \brief SCMPInternalConnectivityDown indicates the AS internal connection 
  between 2 routers is down. The format is as follows:

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|              ISD              |                               |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+         AS                    +
	|                                                               |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                                                               |
	+                   Ingress Interface ID                        +
	|                                                               |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                                                               |
	+                   Egress Interface ID                         +
	|                                                               |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
class SCMPInternalConnectivityDown  : public virtual Layer, public Header
{
public:

SCMPInternalConnectivityDown(){}
SCMPInternalConnectivityDown( IA_t ia, uint64_t ingress, uint64_t egress);


  static LayerType staticLayerType(){ return m_type;}

    LayerType Type() const override ;

    // Contents returns the bytes making up this layer.
    input_t Contents()const override;
    input_t Payload() const override;
    LayerClass CanDecode() const override;
    std::optional<LayerType> NextType() const override;

    uint32_t Deserialize(Buffer::Iterator start) override;

    std::expected<uint32_t,error> Deserialize(input_t start, LayerStore* = nullptr);

    void Serialize( Buffer::Iterator start ) const override;
    output_t Serialize( output_t start, SerializeOptions ) const ;
    uint32_t GetSerializedSize() const ;
     static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;

    void Print(std::ostream& os) const override;


  
private:

    inline static LayerType m_type = LayerTypeRegistry::instance().RegisterLayerType(
                                LayerTypeMetaData{
                                .m_name = "SCMPInternalConnectivityDown",
                                .m_decoder = &decodeSCMPInternalConnectivityDown });

input_t m_start;
input_t m_payload;

IA_t m_ia;

uint64_t m_ingress;
uint64_t m_egress;
};



}
#pragma once
#include "ns3/gopacket++.h"
#include "ns3/scmp-typecodes.h"


namespace ns3
{

std::expected<input_t,error> decodeSCMP( input_t , LayerStore*);

/* MaxSCMPPacketLen the maximum length a SCION packet including SCMP quote can
   have. This length includes the SCION, and SCMP header of the packet.

	+-------------------------+
	|        Underlay         |
	+-------------------------+
	|          SCION          |  \
	|          SCMP           |   \
	+-------------------------+    \_ MaxSCMPPacketLen
	|          Quote:         |    /
	|        SCION Orig       |   /
	|         L4 Orig         |  /
	+-------------------------+
*/
constexpr uint16_t MaxSCMPPacketLen = 1232;
 // this should maybe better become an attribute on SCMPHeader's TypeId

/*!
  \brief SCMP is the SCMP header on top of SCION header.

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|     Type      |     Code      |           Checksum            |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                            InfoBlock                          |
	+                                                               +
	|                         (variable length)                     |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                            DataBlock                          |
	+                                                               +
	|                         (variable length)                     |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
 class SCMPHeader  : public virtual Layer, public Header
 {
public:

    SCMPHeader( SCMPTypeCode tc = 0 ) : m_typecode(tc){} // i think Type 0 is not allocated ?!
    const SCMPTypeCode& TypeCode()const {return m_typecode; }
  static LayerType staticLayerType(){ return m_type;}
    LayerType Type() const override  {     return m_type;   }
    // Contents returns the bytes making up this layer.
    input_t Contents() const override;
    input_t Payload() const override;
    LayerClass CanDecode() const override;
    std::optional<LayerType> NextType() const override;
    uint32_t Deserialize(Buffer::Iterator start) override;
    std::expected<uint32_t,error> Deserialize(input_t start, LayerStore*);
    void Serialize( Buffer::Iterator start ) const override;
    output_t Serialize( output_t start, SerializeOptions ) const ;
    uint32_t GetSerializedSize() const ;
     static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;

    void Print(std::ostream& os) const override;


  
private:

    inline static LayerType m_type = LayerTypeRegistry::instance().RegisterLayerType(LayerTypeMetaData{
                                .m_name = "SCMPHeader",
                                .m_decoder = &decodeSCMP   });


    input_t m_start;
    input_t m_payload;
	 SCMPTypeCode m_typecode;
	uint16_t m_checksum;

	// scn *SCION   SCIONHeader* m_scn; // needed in Go for computation of Checksum
};

}
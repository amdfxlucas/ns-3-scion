#pragma once
#include "ns3/gopacket++.h"
#include "ns3/tlv_option.h"
#include "ns3/scion-types.h"



namespace ns3
{

class ExtnBase
{
    
public:

     // serializeToWithTLVOptions(b gopacket.SerializeBuffer,	opts gopacket.SerializeOptions, tlvOptions []*tlvOption)
     void serializeToWithTLVOptions( output_t, const std::vector<TLVOption> & tlvOptions, const SerializeOptions& opts ) const ;
     //  decodeExtnBase(data []byte, df gopacket.DecodeFeedback)
     std::expected<uint32_t,error> Deserialize( input_t, LayerStore* = nullptr );
    input_t Contents()const{return m_content;}
    input_t Payload()const{ return m_payload; }
    L4ProtocolType_t GetNextHdr()const {return NextHdr; }
    uint32_t GetSerializedSize()const;
    virtual const std::vector<TLVOption>& GetOptions() const = 0;

private:
	L4ProtocolType_t NextHdr ;
	// ExtLen is the length of the extension header in multiple of 4-bytes NOT including the
	// first 4 bytes.
	mutable uint8_t ExtLen  ; // mutable because modified in serializeTo()
	uint32_t ActualLen;

    input_t m_content;
    input_t m_payload;

};

}
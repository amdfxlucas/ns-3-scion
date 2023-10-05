#include "ns3/extn_base.h"
#include "ns3/tlv_option.h"
#include "ns3/basic-error.h"
#include <format>

namespace ns3
{

    std::expected<uint32_t,error> ExtnBase::Deserialize( input_t start, LayerStore* store )
    {
        m_content = start;
        
	if( auto size= start.GetRemainingSize(); size < 2 )
    {
		if(store) {store->set_truncated();}
		return std::unexpected(
			proto_error{ std::format("invalid extension header. Length {} less than 2",	size ) } );
	}

	NextHdr = static_cast<L4ProtocolType_t>( start.ReadU8() );
	ExtLen = start.ReadU8();
	ActualLen = (ExtLen + 1) * LineLen;

    
	
    if( uint32_t size = start.GetRemainingSize(); size < ActualLen )
    {
		return std::unexpected( proto_error{
			std::format("invalid extension header. "
			"Length {} less than specified length {}", size, ActualLen)
	} );
	}
    
	
	m_payload = m_content + ActualLen;

	return 2; // shouldnt this be ActualLen ???!!!
    }

    uint32_t ExtnBase::GetSerializedSize() const
    {
     auto length = serializeTLVOptions(std::nullopt, GetOptions(), /*opts.FixLengths*/ true ); // writes nothing, only computes length   	

	auto 	Ext_Len = uint8_t((length / LineLen) - 1);
    auto actual_len = (int(Ext_Len) + 1) * LineLen;
    return actual_len;
    }

    void ExtnBase::serializeToWithTLVOptions( output_t start, const std::vector<TLVOption> & tlvOptions , const SerializeOptions& opts ) const 
    {
        auto length = serializeTLVOptions(std::nullopt, tlvOptions, opts.FixLengths ); // writes nothing, only computes length

        start.WriteU8( NextHdr );
	
	if( opts.FixLengths )
    {
		ExtLen = uint8_t((length / LineLen) - 1);
	}
	start.WriteU8( ExtLen );

        

        serializeTLVOptions( start, tlvOptions, opts.FixLengths );

        length +=2;
        NS_ASSERT_MSG( length % LineLen == 0 , "SCION Extension actual length must be mutliple of LineLength: 4");


/*
	l := serializeTLVOptions(nil, tlvOptions, opts.FixLengths)
	bytes, err := b.PrependBytes(l)
	if err != nil {
		return err
	}
	serializeTLVOptions(bytes, tlvOptions, opts.FixLengths)

	length := len(bytes) + 2
	if length%LineLen != 0 {
		return serrors.New("SCION extension actual length must be multiple of 4")
	}
	bytes, err = b.PrependBytes(2)
	if err != nil {
		return err
	}
	bytes[0] = uint8(e.NextHdr)
	if opts.FixLengths {
		e.ExtLen = uint8((length / LineLen) - 1)
	}
	bytes[1] = e.ExtLen
	return nil
    
*/
    }

}
#include "ns3/tlv_option.h"
#include "ns3/scion-types.h"
#include "neo/bytewise_iterator.hpp"

#include "neo/buffer_algorithm.hpp"

namespace ns3
{

TLVOption::TLVOption( OptionType type, std::initializer_list<uint8_t> align )
: opt_type(type)
{
    OptAlign[0] = std::data(align)[0];
    OptAlign[1] = std::data(align)[1];
}

    TLVOption::TLVOption(OptionType type, neo::const_buffer data, std::initializer_list<uint8_t> align)
: opt_type(type)
{
    OptAlign[0] = std::data(align)[0];
    OptAlign[1] = std::data(align)[1];
    opt_data.resize( data.size() );
    opt_data = neo::bytes::copy(data);
    opt_data_len = opt_data.size();
}

TLVOption::TLVOption(OptionType type, neo::const_buffer data )
: opt_type(type)
{
opt_data.resize(data.size());
opt_data = neo::bytes::copy(data);
opt_data_len = opt_data.size();
}

  TLVOption::TLVOption( OptionType type, uint8_t len, neo::const_buffer data )
    : opt_type(type),
      opt_data_len(len)
      {
        opt_data.resize( data.size() );
        opt_data =  neo::bytes::copy( data ) ;
       //  auto n = neo::buffer_copy(opt_data,data);
       //  NS_ASSERT(n== opt_data_len);
      }

// serializeTLVOptions serializes options to buf and returns the length of the serialized options.
// Passing in a nil-buffer will treat the serialization as a dryrun that can be used to calculate
// the length needed for the buffer.
int serializeTLVOptions( std::optional<output_t> o_start, range_ref<TLVOption> options ,bool fixLengths ) 
 {

    bool dryrun = !o_start.has_value();
    
	// length start at 2 since the padding needs to be calculated taking the first 2 bytes of the
	// extension header (NextHdr and ExtLen fields) into account.
	int length = 2;
	for ( const auto& opt : options )
    {
		if( fixLengths )
        {
			int x = int(opt.GetOptAlign()[0]);
			int y = int(opt.GetOptAlign()[1]);
			if( x != 0 )
            {
		    	int n = length / x;
				int offset = x*n + y;
				if( offset < length )
                {
					offset += x;
				}
				if ( length != offset )
                {
					int pad = offset - length;
					if (!dryrun )
                    {
						// serializeTLVOptionPadding(buf[length-2:], pad)
                        serializeTLVOptionPadding( o_start.value() + (length-2) , pad);
					}
					length += pad;
				}
			}
		}
		if( !dryrun )
        {
			// opt.Serialize(buf[length-2:], fixLengths);
            opt.Serialize( o_start.value() + (length-2) , fixLengths);
		}
		length += opt.length(fixLengths);
	}
	if( fixLengths)
     {
		int p = length % LineLen;
		if( p != 0 )
        {
			int pad = LineLen - p;
			if( !dryrun )
            {
				// serializeTLVOptionPadding(buf[length-2:], pad);
                serializeTLVOptionPadding( o_start.value() +(length-2), pad);
			}
			length += pad;
		}
	}
	return length - 2;
}

void serializeTLVOptionPadding( output_t start, uint8_t padLength ) 
{
	if( padLength <= 0 )
     {
		return;
	}
	if( padLength == 1 )
    {
		start.WriteU8( 0x0);
		return;
	}
	uint8_t dataLen = padLength - 2;

    TLVOption padN {
		   TLVOption::OptionType::OptTypePadN,
		 dataLen,
		 neo::as_buffer( neo::bytes(dataLen) ),
	};

	padN.Serialize(start, false);
}

/*
input_t
decodeTLVOption(input_t data, LayerStore* pb)
{
    auto scn = std::make_shared<TLVOption>();
    [[maybe_unused]] auto bytes_read = scn->Deserialize(data);
    // err := scn.DecodeFromBytes(data, pb)
    // if err != nil {
    //		return err
    //}
    pb->AddLayer(std::dynamic_pointer_cast<LayerBase>(scn));

    input_t iter = data;

    if (pb->decode_recursive() && scn->NextType().has_value() )
    {
        iter = pb->NextDecoder(scn->NextType().value());
    }
    return iter;
}

input_t
TLVOption::Contents()
{
    return m_content;
}

input_t
TLVOption::Payload()
{
    return m_payload;
}

LayerType
TLVOption::Type() const
{
    return staticLayerType();
}

std::optional<LayerType>
TLVOption::NextType() const
{
    return Payload_t::staticLayerType();
}

LayerClass
TLVOption::CanDecode() const
{
    return LayerClass{{staticLayerType()}};
}

TypeId
TLVOption::GetTypeId()
{
    static TypeId tid = TypeId("ns3::TLVOption")
                            .SetParent<Header>()
                            .SetGroupName("Internet")
                            .AddConstructor<TLVOption>();
    return tid;
}

TypeId
TLVOption::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
TLVOption::Print(std::ostream& os) const
{
    os << "<TLVOption/>";
}

*/

void
TLVOption::Serialize( output_t start, bool fixLengths ) const
{
  
        if (opt_type == OptionType::OptTypePad1) 
        {            
          start.WriteU8( 0x0 );           
         return;
        }
        if (fixLengths) {
            opt_data_len = static_cast<uint8_t>(opt_data.size());
        }
        
        start.WriteU8( static_cast<uint8_t>(opt_type)  );
        start.WriteU8( opt_data_len );

       //  std::copy(OptData.begin(), OptData.end(), data.begin() + 2);

        // auto buff = neo::const_buffer(m_bytes.data(),m_bytes.size());
        auto iter = neo::bytewise_iterator( neo::as_buffer( opt_data ) );

        std::ranges::copy( iter.begin(),iter.end() ,start ) ; 
        
    

}

output_t
TLVOption::Serialize( output_t start, SerializeOptions opts) const
{
      if (opt_type == OptionType::OptTypePad1) 
        {            
          start.WriteU8( 0x0 );           
         return start;
        }
        if ( opts.FixLengths) 
        {
            opt_data_len = static_cast<uint8_t>(opt_data.size());
        }
        
        start.WriteU8( static_cast<uint8_t>(opt_type)  );
        start.WriteU8( opt_data_len );

       //  std::copy(OptData.begin(), OptData.end(), data.begin() + 2);

        // auto buff = neo::const_buffer(m_bytes.data(),m_bytes.size());
        auto iter = neo::bytewise_iterator( neo::as_buffer( opt_data ) );

        std::ranges::copy( iter.begin(),iter.end() ,start ) ; 
        
    
    return start;
}

uint32_t
TLVOption::GetSerializedSize() const
{
      if (opt_type == OptionType::OptTypePad1) 
      {
        return 1;
      }

    // return ActualLength;
    return opt_data.size() + 2;
}

uint32_t
TLVOption::Deserialize(Buffer::Iterator start)
{
    return Deserialize(input_t(start));
}

uint32_t
TLVOption::Deserialize(input_t start)
{
    m_content = start;

    opt_type = static_cast<OptionType>( start.ReadU8() );

    if( static_cast<OptionType>(opt_type) == OptionType::OptTypePad1 )
    {
        ActualLength = 1;
        return 1;
    }
    opt_data_len = start.ReadU8();
    ActualLength= opt_data_len +2;

    opt_data.resize(ActualLength);
    auto n = neo::buffer_copy( neo::as_buffer( opt_data ), neo::as_buffer( &(*(m_content+2)), ActualLength ) );
    NS_ASSERT_MSG( n == static_cast<uint32_t>( ActualLength ), "wrong number of bytes copied" );

    /*o := &tlvOption{OptType: OptionType(data[0])}
	if OptionType(data[0]) == OptTypePad1 {
		o.ActualLength = 1
		return o, nil
	}
	if len(data) < 2 {
		return nil, serrors.New("buffer too short", "expected", 2, "actual", len(data))
	}
	o.OptDataLen = data[1]
	o.ActualLength = int(o.OptDataLen) + 2
	if len(data) < o.ActualLength {
		return nil, serrors.New("buffer too short", "expected", o.ActualLength, "actual", len(data))
	}
	o.OptData = data[2:o.ActualLength]
	return o, nil */

 
	m_payload = m_content + ActualLength;
	
    return ActualLength;
}




}
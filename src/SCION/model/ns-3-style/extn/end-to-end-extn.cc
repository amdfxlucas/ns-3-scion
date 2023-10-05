#include "ns3/end-to-end-extn.h"
#include "ns3/hop-by-hop-extn.h"
#include "ns3/scion-header.h"

namespace ns3
{


// scionNextLayerTypeAfterE2E returns the layer type for the given protocol
// identifier, in a SCION end-to-end extension, excluding (repeated or
// misordered) hop-by-hop extensions or (repeated) end-to-end extensions.
LayerType scionNextLayerTypeAfterE2E( L4ProtocolType_t t) 
{
	switch( t)
    {
	//case HopByHopClass:
	//	return gopacket.LayerTypeDecodeFailure
	//case End2EndClass:
	//	return gopacket.LayerTypeDecodeFailure
	default:
		return scionNextLayerTypeL4(t);
	};
}


std::expected<input_t,error>
decodeEndToEndExtn(input_t data, LayerStore* pb)
{
    auto scn = std::make_shared<EndToEndExtn>();
    input_t iter = data;

    if( auto bytes_read = scn->Deserialize(data); bytes_read)
    {
        iter += *bytes_read;
        pb->AddLayer(std::dynamic_pointer_cast<LayerBase>(scn));
    }else
    {
        return std::unexpected( bytes_read.error() );
    }
    

    if (pb->decode_recursive() && scn->NextType().has_value() )
    {
        if( auto tmp = pb->NextDecoder(scn->NextType().value());tmp)
        {
            iter = *tmp;
        }else
        {
            return std::unexpected( tmp.error() );
        }
    }
    return iter;
}

input_t
EndToEndExtn::Contents() const 
{
    return ExtnBase::Contents();
}

input_t
EndToEndExtn::Payload() const 
{
    return ExtnBase::Payload();
}

LayerType
EndToEndExtn::Type() const
{
    return staticLayerType();
}

std::optional<LayerType>
EndToEndExtn::NextType() const
{
    return scionNextLayerTypeAfterE2E( GetNextHdr() );
}

LayerClass
EndToEndExtn::CanDecode() const
{
    return LayerClass{{staticLayerType()}};
}

void
EndToEndExtn::Serialize(Buffer::Iterator start) const
{
    NS_ASSERT_MSG( GetNextHdr() != HopByHopClass, "EndToEndExtn must not be be succeeded by HopByHopExtn" );
	NS_ASSERT_MSG( GetNextHdr() != End2EndClass, "EndToEndExtn must not be repeated" );

    SerializeOptions opts;
	ExtnBase::serializeToWithTLVOptions(start,  GetOptions(), opts );
}

output_t
EndToEndExtn::Serialize( output_t start, SerializeOptions opts) const
{
    NS_ASSERT_MSG( GetNextHdr() != HopByHopClass, "EndToEndExtn must not be be succeeded by HopByHopExtn" );
	NS_ASSERT_MSG( GetNextHdr() != End2EndClass, "EndToEndExtn must not be repeated" );

   
	ExtnBase::serializeToWithTLVOptions(start, GetOptions() ,opts);
    
    return start;
}

uint32_t
EndToEndExtn::GetSerializedSize() const
{
    return ExtnBase::GetSerializedSize();
}

uint32_t
EndToEndExtn::Deserialize(Buffer::Iterator start)
{
    if( auto tmp = Deserialize(input_t(start),nullptr ); tmp)
    {return *tmp;}
    else
    {
        throw error_exception( tmp.error() );
    }
}

std::expected<uint32_t, error>
EndToEndExtn::Deserialize(input_t start, LayerStore* store)
{
    uint32_t bytes;
    if( auto bytes_read = ExtnBase::Deserialize( start, store ); bytes_read) // will always be == 2
    {
        bytes = *bytes_read;
    } else
    {
        return std::unexpected( bytes_read.error() );
    }
	// h.extnBase, err = decodeExtnBase(data, df)
	
	NS_ASSERT_MSG( GetNextHdr() != End2EndClass, "EndToEndExtn must not be repeated" );
    NS_ASSERT_MSG( GetNextHdr() != HopByHopClass, "EndToEndExtn must not be succeeded by HopByHopExtn" );

	
	for( auto iter= Contents()+bytes; iter < Payload(); )
    {   TLVOption tlvo;
        tlvo.Deserialize(iter);

		/*opt, err := decodeTLVOption(data[offset:h.ActualLen])
		if err != nil {
			return err
		}*/

        m_opts.push_back( tlvo );
        iter+= tlvo.GetSerializedSize();
		// h.Options = append(h.Options, (*EndToEndOption)(opt))
		// offset += opt.ActualLength
	}

	
    return GetSerializedSize();
}

TypeId
EndToEndExtn::GetTypeId()
{
    static TypeId tid = TypeId("ns3::EndToEndExtn")
                            .SetParent<Header>()
                            .SetGroupName("Internet")
                            .AddConstructor<EndToEndExtn>();
    return tid;
}

TypeId
EndToEndExtn::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
EndToEndExtn::Print(std::ostream& os) const
{
    os << "<EndToEndExtn/>";
}



}
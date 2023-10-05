#include "ns3/hop-by-hop-extn.h"
#include "ns3/end-to-end-extn.h"
#include "ns3/scion-header.h"

namespace ns3
{

LayerType scionNextLayerTypeAfterHBH( L4ProtocolType_t t)
{
	switch ( t )
    {
	// case HopByHopClass:
	//	return gopacket.LayerTypeDecodeFailure
	case End2EndClass:
		return EndToEndExtn::staticLayerType();
	default:
		return scionNextLayerTypeL4(t);
	};
}


std::expected<input_t,error>
decodeHopByHopExtn(input_t data, LayerStore* pb)
{
    auto scn = std::make_shared<HopByHopExtn>();
    input_t iter = data;

    if( auto bytes_read = scn->Deserialize(data,pb); bytes_read)
    {
        iter += *bytes_read;
        pb->AddLayer(std::dynamic_pointer_cast<LayerBase>(scn));
    }else
    {
        return std::unexpected( bytes_read.error() );
    }

    

    if (pb->decode_recursive() && scn->NextType().has_value() )
    {
        if( auto tmp= pb->NextDecoder(scn->NextType().value()); tmp)
        {
            iter = *tmp;
        } else
        {
            return std::unexpected(tmp.error() );
        }
    }
    return iter;
}

input_t
HopByHopExtn::Contents() const 
{
    return ExtnBase::Contents();
}

input_t
HopByHopExtn::Payload() const 
{
    return ExtnBase::Payload();
}

LayerType
HopByHopExtn::Type() const
{
    return staticLayerType();
}

std::optional<LayerType>
HopByHopExtn::NextType() const
{
    return scionNextLayerTypeAfterHBH( GetNextHdr() );
}

LayerClass
HopByHopExtn::CanDecode() const
{
    return LayerClass{{staticLayerType()}};
}

void
HopByHopExtn::Serialize(Buffer::Iterator start) const
{

	NS_ASSERT_MSG( GetNextHdr() != HopByHopClass, "HopByHopExtn must not be repeated" );

    SerializeOptions opts;
	ExtnBase::serializeToWithTLVOptions(start, GetOptions(),opts );
}

output_t
HopByHopExtn::Serialize( output_t start, SerializeOptions opts) const
{
	NS_ASSERT_MSG( GetNextHdr() != HopByHopClass, "HopByHopExtn must not be repeated" );

   
	ExtnBase::serializeToWithTLVOptions(start,  GetOptions(), opts );
    
    return start;
}

uint32_t
HopByHopExtn::GetSerializedSize() const
{
    return ExtnBase::GetSerializedSize();
}

uint32_t
HopByHopExtn::Deserialize(Buffer::Iterator start)
{
    if(auto tmp = Deserialize(input_t(start)); tmp)
    {return *tmp; }
    else
    {
        throw error_exception(tmp.error() );
    }
}

std::expected<uint32_t,error>
HopByHopExtn::Deserialize(input_t start, LayerStore* store)
{

    uint32_t bytes;
  if(  auto bytes_read = ExtnBase::Deserialize( start,store );bytes_read )// will always be == 2
  {
    bytes = *bytes_read;
  } else
  {
    return std::unexpected( bytes_read.error() );
  }

	// h.extnBase, err = decodeExtnBase(data, df)
	
	NS_ASSERT_MSG( GetNextHdr() != HopByHopClass, "HopByHopExtn must not be repeated" );

	
	for( auto iter= Contents()+bytes; iter < Payload(); )
    {   TLVOption tlvo;
        tlvo.Deserialize(iter);

		/*opt, err := decodeTLVOption(data[offset:h.ActualLen])
		if err != nil {
			return err
		}*/

        m_opts.push_back( tlvo );
        iter+= tlvo.GetSerializedSize();
		// h.Options = append(h.Options, (*HopByHopOption)(opt))
		// offset += opt.ActualLength
	}

	
    return GetSerializedSize();
}

TypeId
HopByHopExtn::GetTypeId()
{
    static TypeId tid = TypeId("ns3::HopByHopExtn")
                            .SetParent<Header>()
                            .SetGroupName("Internet")
                            .AddConstructor<HopByHopExtn>();
    return tid;
}

TypeId
HopByHopExtn::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
HopByHopExtn::Print(std::ostream& os) const
{
    os << "<HopByHopExtn/>";
}



}
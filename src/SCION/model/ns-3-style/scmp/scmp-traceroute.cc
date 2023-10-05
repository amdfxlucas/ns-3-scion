#include "ns3/scmp-traceroute.h"
#include "ns3/basic-error.h"

namespace ns3
{

std::expected<input_t,error>
decodeSCMPTraceroute(input_t data, LayerStore* pb)
{
    auto scn = std::make_shared<SCMPTraceroute>();
    input_t iter = data;

    if( auto bytes_read = scn->Deserialize(data, pb); bytes_read )
    {
        iter += *bytes_read;
    pb->AddLayer(std::dynamic_pointer_cast<LayerBase>(scn));
    }else
    {
        return std::unexpected( bytes_read.error() );
    }

    

    if (pb->decode_recursive() && scn->NextType().has_value() )
    {
       if( auto tmp = pb->NextDecoder(scn->NextType().value()); tmp)
       {
            iter  = *tmp;
       }
       else
       {
            return std::unexpected( tmp.error() );
       }

    }
    return iter;
}

input_t
SCMPTraceroute::Contents() const
{
    return m_start;
}

input_t
SCMPTraceroute::Payload() const
{
    return m_payload;
}

LayerType
SCMPTraceroute::Type() const
{
    return staticLayerType();
}

std::optional<LayerType>
SCMPTraceroute::NextType() const
{
    return Payload_t::staticLayerType();
}

LayerClass
SCMPTraceroute::CanDecode() const
{
    return LayerClass{{staticLayerType()}};
}

void
SCMPTraceroute::Serialize(Buffer::Iterator start) const
{
       start.WriteHtonU16(m_identifier);
start.WriteHtonU16(m_sequence);
start.WriteHtonU64(m_ia);
start.WriteHtonU64(m_interface);

}

output_t
SCMPTraceroute::Serialize( output_t start, SerializeOptions) const
{

    start.WriteHtonU16(m_identifier);
start.WriteHtonU16(m_sequence);
start.WriteHtonU64(m_ia);
start.WriteHtonU64(m_interface);

    return start;
}

uint32_t
SCMPTraceroute::GetSerializedSize() const
{
    return 20;
}

uint32_t
SCMPTraceroute::Deserialize(Buffer::Iterator start)
{
    if( auto tmp= Deserialize(input_t(start)); tmp)
    {
        return *tmp;
    }else
    {
        throw error_exception(tmp.error() );
    }
}

std::expected<uint32_t,error>
SCMPTraceroute::Deserialize(input_t start, LayerStore* store )
{
    m_start = start;

	if( uint32_t size = start.GetRemainingSize(); size < GetSerializedSize() )
     {
		if(store){ store->set_truncated(); }
		return std::unexpected( basic_error{"buffer too short",
         "min", std::to_string(GetSerializedSize() ),
          "actual", std::to_string(size) } );
	}
	
	m_identifier = start.ReadNtohU16();	
    m_sequence = start.ReadNtohU16();	
	m_ia = start.ReadNtohU64();	
    m_interface = start.ReadNtohU64();	
	m_payload = start;

    return GetSerializedSize() ;
}

TypeId
SCMPTraceroute::GetTypeId()
{
    static TypeId tid = TypeId("ns3::SCMPTraceroute")
                            .SetParent<Header>()
                            .SetGroupName("Internet")
                            .AddConstructor<SCMPTraceroute>();
    return tid;
}

TypeId
SCMPTraceroute::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
SCMPTraceroute::Print(std::ostream& os) const
{
    os << "<SCMPTraceroute/>";
}

}
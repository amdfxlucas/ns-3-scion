#include "ns3/scmp-echo.h"
#include "ns3/basic-error.h"

namespace ns3
{

std::expected<input_t,error>
decodeSCMPEcho(input_t data, LayerStore* pb)
{
    auto scn = std::make_shared<SCMPEcho>();

    input_t iter = data;
    
    if(  auto bytes_read = scn->Deserialize(data, pb); bytes_read )
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
            iter = *tmp;
        } else
        {
            return std::unexpected( tmp.error() );
        }
    }
    return iter;
}

input_t
SCMPEcho::Contents() const
{
    return m_content;
}

input_t
SCMPEcho::Payload() const
{
    return m_payload;
}

LayerType
SCMPEcho::Type() const
{
    return staticLayerType();
}

std::optional<LayerType>
SCMPEcho::NextType() const
{
    return Payload_t::staticLayerType();
}

LayerClass
SCMPEcho::CanDecode() const
{
    return LayerClass{{staticLayerType()}};
}

void
SCMPEcho::Serialize(Buffer::Iterator start) const
{
    start.WriteHtonU16( m_identifier );
    start.WriteHtonU16( m_sequence_number );
}

output_t
SCMPEcho::Serialize( output_t start, SerializeOptions) const
{
    start.WriteHtonU16( m_identifier );
    start.WriteHtonU16( m_sequence_number );
    
    return start;
}

uint32_t
SCMPEcho::GetSerializedSize() const
{
    return 4;
}

uint32_t
SCMPEcho::Deserialize(Buffer::Iterator start)
{
    if( auto tmp = Deserialize(input_t(start)); tmp)
    {
        return *tmp;
    }else
    {
        throw error_exception( tmp.error() );
    }
}

std::expected<uint32_t,error>
SCMPEcho::Deserialize(input_t start, LayerStore* store )
{
    
	if( uint32_t size = start.GetRemainingSize(),minLen = 4 ; size < minLen)
    {
		if(store) { store->set_truncated();}
		return std::unexpected( 
            basic_error{"buffer too short",
         "min", std::to_string( minLen ),
          "actual", std::to_string(size )
          });
	}
	m_content = start;
	m_identifier = start.ReadNtohU16();
	
	m_sequence_number = start.ReadNtohU16();
	m_payload = start;
	
    return 4;
}

TypeId
SCMPEcho::GetTypeId()
{
    static TypeId tid = TypeId("ns3::SCMPEcho")
                            .SetParent<Header>()
                            .SetGroupName("Internet")
                            .AddConstructor<SCMPEcho>();
    return tid;
}

TypeId
SCMPEcho::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
SCMPEcho::Print(std::ostream& os) const
{
    os << "<SCMPEcho/>";
}


}
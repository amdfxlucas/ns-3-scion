#include "ns3/scmp-parameter-problem.h"
#include "ns3/basic-error.h"

namespace ns3
{

std::expected<input_t,error>
decodeSCMPParameterProblem(input_t data, LayerStore* pb)
{
    auto scn = std::make_shared<SCMPParameterProblem>();
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
        if( auto tmp = pb->NextDecoder(scn->NextType().value()); tmp)
        {
            iter = *tmp;
        }
        else
        {
            return std::unexpected( tmp.error() );
        }
    }
    return iter;
}

input_t
SCMPParameterProblem::Contents() const
{
    return m_start;
}

input_t
SCMPParameterProblem::Payload() const
{
    return m_payload;
}

LayerType
SCMPParameterProblem::Type() const
{
    return staticLayerType();
}

std::optional<LayerType>
SCMPParameterProblem::NextType() const
{
    return Payload_t::staticLayerType();
}

LayerClass
SCMPParameterProblem::CanDecode() const
{
    return LayerClass{{staticLayerType()}};
}

void
SCMPParameterProblem::Serialize(Buffer::Iterator start) const
{
   
   start.WriteHtonU16(0);
   start.WriteHtonU16(m_pointer);
}

output_t
SCMPParameterProblem::Serialize( output_t start, SerializeOptions) const
{
   start.WriteHtonU16(0);
   start.WriteHtonU16(m_pointer);

    return start;
}

uint32_t
SCMPParameterProblem::GetSerializedSize() const
{
    return 4;
}

uint32_t
SCMPParameterProblem::Deserialize(Buffer::Iterator start)
{
    if(auto tmp = Deserialize(input_t(start)); tmp)
    {
        return *tmp;
    } else
    {
        throw error_exception( tmp.error() );
    }
}

std::expected<uint32_t,error>
SCMPParameterProblem::Deserialize(input_t start, LayerStore* store )
{
    m_start = start;
    
	if( uint32_t size = start.GetRemainingSize(); size < GetSerializedSize() )
     {
		if(store){ store->set_truncated(); }

		return std::unexpected( basic_error{"buffer too short",
         "min", std::to_string( GetSerializedSize() ),
          "actual", std::to_string(size )
          });
    }

    start+=2;
    m_pointer = start.ReadNtohU16();
	
	m_payload = start;

    return 4;
}

TypeId
SCMPParameterProblem::GetTypeId()
{
    static TypeId tid = TypeId("ns3::SCMPParameterProblem")
                            .SetParent<Header>()
                            .SetGroupName("Internet")
                            .AddConstructor<SCMPParameterProblem>();
    return tid;
}

TypeId
SCMPParameterProblem::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
SCMPParameterProblem::Print(std::ostream& os) const
{
    os << "<SCMPParameterProblem/>";
}


}
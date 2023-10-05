#include "ns3/scmp-external-interface-down.h"

namespace ns3
{

std::expected<input_t,error>
decodeSCMPExternalInterfaceDown(input_t data, LayerStore* pb)
{
    auto scn = std::make_shared<SCMPExternalInterfaceDown>();
    input_t iter = data;

    if( auto bytes_read = scn->Deserialize(data); bytes_read )
    {
    iter += *bytes_read;
    pb->AddLayer(std::dynamic_pointer_cast<LayerBase>(scn));
    }else
    {
        return std::unexpected( bytes_read.error() );
    }

    

    if (pb->decode_recursive() && scn->NextType().has_value() )
    {
        if(auto tmp = pb->NextDecoder(scn->NextType().value()); tmp)
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
SCMPExternalInterfaceDown::Contents() const
{
    return m_start;
}

input_t
SCMPExternalInterfaceDown::Payload() const
{
    return m_payload;
}

LayerType
SCMPExternalInterfaceDown::Type() const
{
    return staticLayerType();
}

std::optional<LayerType>
SCMPExternalInterfaceDown::NextType() const
{
    return Payload_t::staticLayerType();
}

LayerClass
SCMPExternalInterfaceDown::CanDecode() const
{
    return LayerClass{{staticLayerType()}};
}

void
SCMPExternalInterfaceDown::Serialize(Buffer::Iterator start) const
{
    start.WriteHtonU64(m_ia);
    start.WriteHtonU64(m_ifid);
}

output_t
SCMPExternalInterfaceDown::Serialize( output_t start, SerializeOptions) const
{
    start.WriteHtonU64(m_ia);
    start.WriteHtonU64(m_ifid);
    return start;
}

uint32_t
SCMPExternalInterfaceDown::GetSerializedSize() const
{
    return 16;
}

uint32_t
SCMPExternalInterfaceDown::Deserialize(Buffer::Iterator start)
{
    if( auto tmp =  Deserialize(input_t(start));tmp)
    {
        return *tmp;
    } else
    {
        throw error_exception( tmp.error() );
    }
}

std::expected<uint32_t,error>
SCMPExternalInterfaceDown::Deserialize(input_t start, LayerStore* store )
{
    m_ia = start.ReadNtohU64();
    m_ifid = start.ReadNtohU64();
    return 16;
}

TypeId
SCMPExternalInterfaceDown::GetTypeId()
{
    static TypeId tid = TypeId("ns3::SCMPExternalInterfaceDown")
                            .SetParent<Header>()
                            .SetGroupName("Internet")
                            .AddConstructor<SCMPExternalInterfaceDown>();
    return tid;
}

TypeId
SCMPExternalInterfaceDown::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
SCMPExternalInterfaceDown::Print(std::ostream& os) const
{
    os << "<SCMPExternalInterfaceDown/>";
}

} // namespace ns3
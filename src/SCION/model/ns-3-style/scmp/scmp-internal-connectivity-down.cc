#include "ns3/scmp-internal-connectivity-down.h"
#include "ns3/basic-error.h"

namespace ns3
{

SCMPInternalConnectivityDown::SCMPInternalConnectivityDown( IA_t ia, uint64_t ingress, uint64_t egress)
: m_ia(ia),
m_ingress(ingress),
m_egress(egress)
{

}

std::expected<input_t, error>
decodeSCMPInternalConnectivityDown(input_t data, LayerStore* pb)
{
    auto scn = std::make_shared<SCMPInternalConnectivityDown>();
    input_t iter = data;
    if( auto bytes_read = scn->Deserialize(data,pb); bytes_read )
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
        }else
        {
            return tmp;
        }
    }
    return iter;
}

input_t
SCMPInternalConnectivityDown::Contents()const
{
    return m_start;
}

input_t
SCMPInternalConnectivityDown::Payload() const
{
    return m_payload;
}

LayerType
SCMPInternalConnectivityDown::Type() const
{
    return staticLayerType();
}

std::optional<LayerType>
SCMPInternalConnectivityDown::NextType() const
{
    return Payload_t::staticLayerType();
}

LayerClass
SCMPInternalConnectivityDown::CanDecode() const
{
    return LayerClass{{staticLayerType()}};
}

void
SCMPInternalConnectivityDown::Serialize(Buffer::Iterator start) const
{
   
   start.WriteHtonU64(m_ia);
    start.WriteHtonU64(m_ingress);
    start.WriteHtonU64(m_egress);
}

output_t
SCMPInternalConnectivityDown::Serialize( output_t start, SerializeOptions) const
{
    start.WriteHtonU64(m_ia);
    start.WriteHtonU64(m_ingress);
    start.WriteHtonU64(m_egress);

    return start;
}

uint32_t
SCMPInternalConnectivityDown::GetSerializedSize() const
{
    return 24;
}

uint32_t
SCMPInternalConnectivityDown::Deserialize(Buffer::Iterator start)
{
    if( auto tmp = Deserialize(input_t(start)); tmp)
    { return *tmp;
    }else
    {
        throw error_exception( tmp.error() );
    }
}

std::expected< uint32_t, error>
SCMPInternalConnectivityDown::Deserialize(input_t start, LayerStore* store)
{
    m_start = start;    
	
	if( uint32_t size = start.GetRemainingSize(); size < GetSerializedSize() )
     {
	    if(store){store->set_truncated();}
		return std::unexpected( basic_error{"buffer too short",
         "mininum_legth", std::to_string( GetSerializedSize() ),
          "actual", std::to_string(size) } );
	} 
    
	
    m_ia = start.ReadNtohU64();	
    m_ingress = start.ReadNtohU64();
    m_egress = start.ReadNtohU64();

    return 24;
}

TypeId
SCMPInternalConnectivityDown::GetTypeId()
{
    static TypeId tid = TypeId("ns3::SCMPInternalConnectivityDown")
                            .SetParent<Header>()
                            .SetGroupName("Internet")
                            .AddConstructor<SCMPInternalConnectivityDown>();
    return tid;
}

TypeId
SCMPInternalConnectivityDown::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
SCMPInternalConnectivityDown::Print(std::ostream& os) const
{
    os << "<SCMPInternalConnectivityDown/>";
}

}
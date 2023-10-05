#include "ns3/scmp-packet-too-big.h"
#include "ns3/basic-error.h"

namespace ns3
{

   std::expected<input_t,error> decodeSCMPPacketTooBig(input_t data, LayerStore* pb)
    {
    auto scn = std::make_shared<SCMPPacketTooBig>();
    input_t iter = data;

    if( auto bytes_read = scn->Deserialize(data); bytes_read )
    {
        iter += *bytes_read;
    pb->AddLayer(std::dynamic_pointer_cast<LayerBase>(scn));
    }
	

	if( pb->decode_recursive() && scn->NextType().has_value() )
    {
		if( auto tmp=  pb->NextDecoder(scn->NextType().value() ); tmp)
        {
            iter = *tmp;
        }else
        {
            return std::unexpected( tmp.error() );
        }
	}	
	return iter;
    }


    // Contents returns the bytes making up this layer.
    input_t SCMPPacketTooBig::Contents() const
    {
        return m_start;
    }

    input_t SCMPPacketTooBig::Payload() const
    {
        return m_payload;
    }

    LayerClass SCMPPacketTooBig::CanDecode() const
    {
        return LayerClass{ {staticLayerType()} };
    }

    std::optional<LayerType> SCMPPacketTooBig::NextType() const
    {
        return Payload_t::staticLayerType();
    }

    uint32_t SCMPPacketTooBig::Deserialize(Buffer::Iterator start) 
    {
        if(auto tmp= Deserialize( input_t(start) ); tmp)
        {
            return *tmp;
        }else
        {
            throw error_exception( tmp.error() );
        }
    }

    std::expected<uint32_t,error> SCMPPacketTooBig::Deserialize(input_t start, LayerStore* store )
    {


	if(auto size = start.GetRemainingSize(); size < GetSerializedSize() )
    {
		if( store ){ store->set_truncated();}
		return std::unexpected(
            basic_error{"buffer too short",
             "min", std::to_string(GetSerializedSize() ),
              "actual", std::to_string(size) } );
	}
	
	
  m_start = start;
    start += 2;
    m_mtu = start.ReadNtohU16();
    m_payload = start;
    return 4;

    }

    void SCMPPacketTooBig::Serialize( Buffer::Iterator start ) const
    {
        start.WriteHtonU16(0);
        start.WriteHtonU16(m_mtu);
    }

    output_t SCMPPacketTooBig::Serialize( output_t start , SerializeOptions ) const 
    {
        start.WriteHtonU16(0);
        start.WriteHtonU16(m_mtu);
        return start;
    }

    uint32_t SCMPPacketTooBig::GetSerializedSize() const 
    {
        return 4;
    }

     TypeId SCMPPacketTooBig::GetTypeId()
     {
  static TypeId tid = TypeId("ns3::SCMPPacketTooBig")
                            .SetParent<Header>()
                            .SetGroupName("Internet")
                             .AddConstructor<SCMPPacketTooBig>();
    return tid;
     }

    TypeId SCMPPacketTooBig::GetInstanceTypeId() const
    {
        return GetTypeId();
    }

    void SCMPPacketTooBig::Print(std::ostream& os) const
    {
        os << "<SCMPPacketTooBig/>";
    }

    
}
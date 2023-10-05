#include "ns3/scmp-destination-unreachable.h"
#include "ns3/basic-error.h"

namespace ns3
{

   std::expected<input_t,error> decodeSCMPDestinationUnreachable(input_t data, LayerStore* pb )
    {
    auto scn = std::make_shared<SCMPDestinationUnreachable>();
input_t iter = data;
    
    if( auto bytes_read = scn->Deserialize(data,pb); bytes_read)
    {
       iter += *bytes_read;
        pb->AddLayer(std::dynamic_pointer_cast<LayerBase>(scn));
    }else
    {
        return std::unexpected( bytes_read.error() );
    }
    
    
    
	

	if( pb->decode_recursive() && scn->NextType().has_value() )
    {
		if( auto tmp = pb->NextDecoder( scn->NextType().value() ); tmp)
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

    input_t SCMPDestinationUnreachable::Contents() const
    {
        return m_start;
    }

    input_t SCMPDestinationUnreachable::Payload() const
    {
        return m_payload;
    }

    LayerClass SCMPDestinationUnreachable::CanDecode() const
    {
        return LayerClass{ {staticLayerType()} };
    }

     std::optional<LayerType> SCMPDestinationUnreachable::NextType() const
     {
        return Payload_t::staticLayerType();
     }

    uint32_t SCMPDestinationUnreachable::Deserialize(Buffer::Iterator start) 
    {
        if( auto tmp = Deserialize( input_t( start ),nullptr );  tmp )
        {
            return *tmp;
        }
        else
        {
            throw error_exception( tmp.error() );
        }

    }

    std::expected<uint32_t, error> SCMPDestinationUnreachable::Deserialize(input_t start, LayerStore* store)
    {
        
	if( uint32_t size = start.GetRemainingSize(), minLen = 4; size < minLen )
    {
		if(store){store->set_truncated();}

		return std::unexpected( basic_error{"buffer too short",
         "min", std::to_string(minLen),
         "actual", std::to_string(size)
        });
	}

      m_start = start;
      start.ReadU32();
      m_payload = start+4;
      return 4;
    }

    void SCMPDestinationUnreachable::Serialize( Buffer::Iterator start ) const 
    {   
        start.WriteU32(0);
    }

    output_t SCMPDestinationUnreachable::Serialize( output_t start, SerializeOptions ) const 
    {
        start.WriteU32(0);
        return start;
    }

    uint32_t SCMPDestinationUnreachable::GetSerializedSize() const 
    {
        return 4;
    }

      TypeId SCMPDestinationUnreachable::GetTypeId()
     {
        static TypeId tid = TypeId("ns3::SCMPDestinationUnreachable")
                            .SetParent<Header>()
                            .SetGroupName("Internet")
                            .AddConstructor<SCMPDestinationUnreachable>();
    return tid;
     }

    TypeId SCMPDestinationUnreachable::GetInstanceTypeId() const
    {
        return GetTypeId();
    }

    void SCMPDestinationUnreachable::Print(std::ostream& os) const 
    {
        os << "<SCMPDestinationUnreachable/>";
    }

}
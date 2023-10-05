#include "ns3/scmp-header.h"

#include "ns3/scmp-packet-too-big.h"
#include "ns3/scmp-external-interface-down.h"
#include "ns3/scmp-destination-unreachable.h"
#include "ns3/scmp-internal-connectivity-down.h"
#include "ns3/scmp-traceroute.h"
#include "ns3/scmp-parameter-problem.h"
#include "ns3/scmp-echo.h"
#include "ns3/basic-error.h"

namespace ns3
{

std::expected<input_t,error>
decodeSCMP(input_t data, LayerStore* pb)
{
    auto scn = std::make_shared<SCMPHeader>();
    input_t iter = data;

   if( [[maybe_unused]] auto bytes_read = scn->Deserialize(data,pb); bytes_read)
   {
    iter += *bytes_read;
    pb->AddLayer(std::dynamic_pointer_cast<LayerBase>(scn));
   } else
   {
    return std::unexpected( bytes_read.error() );
   }   

    // an SCMPHeader alone is useless -> it always needs a more specific SCMPCode Layer
    if ( /*pb->decode_recursive() &&*/ scn->NextType().has_value() )
    {

        if( auto tmp  = pb->NextDecoder(scn->NextType().value()); tmp )
        {
         iter = *tmp;
        }
        else
        {
            return std::unexpected( tmp.error() );
        }
    }else
    {
        return std::unexpected( proto_error{"special second scmp header expected but none was provided"} );
    }

    return iter;
}

// Contents returns the bytes making up this layer.
input_t
SCMPHeader::Contents() const
{
    return m_start;
}

input_t
SCMPHeader::Payload() const
{
    return m_payload;
}

LayerClass
SCMPHeader::CanDecode() const
{
 return LayerClass{ {staticLayerType()} };
}

std::optional<LayerType>
SCMPHeader::NextType() const
{
    
    switch (TypeCode().Type())
    {
    
    case SCMP::TypePacketTooBig:
        return SCMPPacketTooBig::staticLayerType();
    case SCMP::TypeExternalInterfaceDown:
        return SCMPExternalInterfaceDown::staticLayerType();
    case SCMP::TypeEchoRequest:
    case SCMP::TypeEchoReply:
        return SCMPEcho::staticLayerType();
    case SCMP::TypeDestinationUnreachable:
        return SCMPDestinationUnreachable::staticLayerType();
     case SCMP::TypeParameterProblem:
        return SCMPParameterProblem::staticLayerType();
      case SCMP::TypeInternalConnectivityDown:
        return SCMPInternalConnectivityDown::staticLayerType();     
    case SCMP::TypeTracerouteRequest:
    case SCMP::TypeTracerouteReply:
        return SCMPTraceroute::staticLayerType();
    };
    

    return Payload_t::staticLayerType();
}

uint32_t
SCMPHeader::Deserialize(Buffer::Iterator start)
{
    if(auto tmp = Deserialize(input_t(start),nullptr); tmp)
    {
        return *tmp;
    }else
    {
        throw error_exception( tmp.error() );
    }
}

std::expected<uint32_t,error>
SCMPHeader::Deserialize(input_t start, LayerStore* store)
{
    
        if( auto size = start.GetRemainingSize() ; size < 4 )
        {
          if(store){store->set_truncated();}
            return std::unexpected( basic_error{"SCMP layer length is less then 4 bytes",
             "minimum", "4",
              "actual",std::to_string(size) } );
        }
    
    m_start = start;
    auto fst = start.ReadU8();
    auto snd = start.ReadU8();
    m_typecode = SCMPTypeCode(fst, snd);
    m_checksum = start.ReadNtohU16(); // =binary.BigEndian.Uint16(data[2:4])

    m_payload = start;
    return 4;
}

void
SCMPHeader::Serialize(Buffer::Iterator start) const
{
   	
	start.WriteHtonU16(m_typecode.TypeCode() );

/*	if opts.ComputeChecksums
    {
		if s.scn == nil {
			return serrors.New("can not calculate checksum without SCION header")
		}
		// zero out checksum bytes
		//bytes[2] = 0
		//bytes[3] = 0
        start.WriteHtonU16(0);
		m_checksum, err = s.scn.computeChecksum(b.Bytes(), uint8(L4SCMP))



		if err != nil {
			return err
		}

	}
*/    
	start.WriteHtonU16( m_checksum);
	
}

output_t
SCMPHeader::Serialize( output_t start, SerializeOptions) const
{
   
   	
	start.WriteHtonU16(m_typecode.TypeCode() );

/*	if opts.ComputeChecksums
    {
		if s.scn == nil {
			return serrors.New("can not calculate checksum without SCION header")
		}
		// zero out checksum bytes
		//bytes[2] = 0
		//bytes[3] = 0
        start.WriteHtonU16(0);
		m_checksum, err = s.scn.computeChecksum(b.Bytes(), uint8(L4SCMP))
        


		if err != nil {
			return err
		}

	}
*/    
	start.WriteHtonU16( m_checksum);

    return start;
}

uint32_t
SCMPHeader::GetSerializedSize() const
{
    return 4;
}

TypeId
SCMPHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::SCMPHeader")
                            .SetParent<Header>()
                            .SetGroupName("Internet");
                            // .AddConstructor<SCMPHeader>();
    return tid;
}

TypeId
SCMPHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
SCMPHeader::Print(std::ostream& os) const
{
    os << "SCMPHeader";
}

} // namespace ns3
#include "ns3/udp-layer.h"

namespace ns3
{

LayerType
NextApplicationLayerType(uint16_t dstPort)
{
    /*
    switch( dstPort )
    {
        case 587:
        case 465:
        case 25:
        return SMTPLayer::staticLayerType();
    case 53:
     return DNSLayer::staticLayerType();
    };
    */

    return Payload_t::staticLayerType();
}

std::expected<input_t , error>
decodeUDPLayer(input_t data, LayerStore* pb)
{
    auto scn = std::make_shared<UDPLayer>();
    input_t iter = data;

    if( auto bytes_read = scn->Deserialize(data,pb); bytes_read )
    {
        iter += *bytes_read;
        pb->AddLayer(std::dynamic_pointer_cast<LayerBase>(scn));
        // pb.SetNetworkLayer(scn)
    } else
    {
        return std::unexpected( bytes_read.error() );
    }
    

    // a UDP layer makes no sense alone without its payload
    if ( /*pb->decode_recursive() && */ scn->NextType().has_value() )
    {
        if( auto tmp = pb->NextDecoder(scn->NextType().value() ); tmp)
        {
            iter = *tmp;
        } else
        {
            return std::unexpected(tmp.error() );
        }
        // this is NOT the responsibility of the LayerType, but the Packet !!
        // in Packet::InitialDecode the whole packet will get parsed, instead of only the first
    }

    return iter;
}

TypeId
UDPLayer::GetTypeId()
{
    static TypeId tid = TypeId("ns3::UDPLayer").SetParent<Header>().SetGroupName("Internet");
    return tid;
}

/*can only be called on headers which have been deserialized from a packet */
uint16_t
UDPLayer::GetPayloadSize() const
{
    return m_payloadSize;
}

std::optional<LayerType>
UDPLayer::NextType() const 
{
    // gopacket returns a layer associated with DestinationPort here
    // i.e. DNS for 53 etc.

    // return Payload_t::staticLayerType();
    return NextApplicationLayerType( GetDestinationPort() );
}

std::expected<uint32_t,error>
UDPLayer::Deserialize(input_t start, LayerStore* )
{
    m_start = start;
    auto i = start;
    m_sourcePort = i.ReadNtohU16();
    m_destinationPort = i.ReadNtohU16();
    m_payloadSize = i.ReadNtohU16(); // - GetSerializedSize();
    m_checksum = i.ReadU16();

    /*if (m_calcChecksum)
    {
        uint16_t headerChecksum = CalculateHeaderChecksum(start.size()); // start.GetSize()
        i = start;
        uint16_t checksum = i.CalculateIpChecksum(start.size(), headerChecksum); // start.GetSize()

        m_goodChecksum = (checksum == 0);
    }*/

    return UdpHeader::GetSerializedSize();
}

output_t
UDPLayer::Serialize(output_t start, [[maybe_unused]] SerializeOptions opt) const
{
    // if( opt.ComputeChecksum ){ EnableChecksums();}

    auto i = start;

    i.WriteHtonU16(GetSourcePort());
    i.WriteHtonU16(GetDestinationPort());
    if (m_forcedPayloadSize == 0)
    {
        //  i.WriteHtonU16(start.GetSize());
        i.WriteHtonU16(i.GetRemainingSize());
    }
    else
    {
        i.WriteHtonU16(m_forcedPayloadSize);
    }

    if (m_checksum == 0)
    {
        i.WriteU16(0);

        /*if (opt.ComputeChecksums)
        {
            uint16_t headerChecksum = CalculateHeaderChecksum(start.size()); // start.GetSize()
            i = start;
            uint16_t checksum = i.CalculateIpChecksum(start.size(), headerChecksum); // start.GetSize()

            i = start;
            i+=6;
            i.WriteU16(checksum);
        }*/
    }
    else /// checksum was forced
    {
        i.WriteU16(m_checksum);
    }
    return i;
}

} // namespace ns3
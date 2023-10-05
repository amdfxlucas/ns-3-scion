#include "ns3/scion-header.h"

#include "ns3/address-utils.h"
#include "ns3/endian-utils.h"
#include "ns3/scmp-header.h"
#include "ns3/scion-address.h"
#include "ns3/hop-by-hop-extn.h"
#include "ns3/end-to-end-extn.h"

#include "ns3/raw-path.h"
#include "ns3/basic-error.h"
#include "ns3/decoded-path.h"
#include "ns3/udp-layer.h"
// #include "ns3/one-hop-path.h"

namespace ns3
{

LayerClass
SCIONHeader::CanDecode() const
{
    return LayerClass{{m_type}};
}

// scionNextLayerTypeL4 returns the layer type for the given layer-4 protocol identifier.
// Does not handle extension header classes.
LayerType
scionNextLayerTypeL4([[maybe_unused]] L4ProtocolType_t t)
{
    	switch ( t )
    	{   
    case L4UDP:
        return UDPLayer::staticLayerType();
    case L4SCMP:
        return SCMPHeader::staticLayerType();
    //case L4BFD:
    //    return layerTypeBFD;
    default:
    
    return Payload_t::staticLayerType();
    	};
}

// scionNextLayerType returns the layer type for the given protocol identifier
// in a SCION base header.
LayerType
scionNextLayerType(L4ProtocolType_t t)
{
    switch (t)
    {
    
    case HopByHopClass:
        return HopByHopExtn::staticLayerType();
    case End2EndClass:
        return EndToEndExtn::staticLayerType();
        
    default:
        return scionNextLayerTypeL4(t);
    };
}


std::expected<input_t,error>
decodeSCION(input_t data, LayerStore* pb)
{
    auto scn = std::make_shared<SCIONHeader>();

    uint32_t bytes;
    if( auto bytes_read = scn->Deserialize(data,pb); bytes_read)
    {
        bytes= *bytes_read;
        pb->AddLayer(std::dynamic_pointer_cast<LayerBase>(scn));
    // pb.SetNetworkLayer(scn)
    }else
    {
        return std::unexpected( bytes_read.error() );
    }
    
    
	input_t iter = data;

	if( pb->decode_recursive() )
    {
		if( auto tmp = pb->NextDecoder(scionNextLayerType( scn->NextHeader())); tmp )
        {
            iter= *tmp;
        } else
        {
            return std::unexpected( tmp.error() );
        }

		// this is NOT the responsibility of the LayerType, but the Packet !!
    // in Packet::InitialDecode the whole packet will get parsed, instead of only the first
	} else
    {
        iter += bytes;
    }

	
	return iter;
}

LayerType
SCIONHeader::Type() const
{
    return m_type;
}

std::optional<LayerType>
SCIONHeader::NextType() const
{
    return scionNextLayerType(NextHeader());
}

output_t SCIONHeader::Serialize( output_t start, SerializeOptions opts ) const
{
	
	auto iter_to_begin = start; // only for debug assertions

    uint32_t scnLen = CmnHdrLen + AddrHdrLen() + _path.Len();
    NS_ASSERT_MSG(scnLen < MaxHdrLen,
    "The ScionHeader must not exceede a max of " << MaxHdrLen << " bytes");
    
    NS_ASSERT_MSG((scnLen % LineLen) == 0,
    "The ScionHeader length must be a multiple of 4 bytes but is: "<< scnLen);    

    NS_ASSERT_MSG( start.GetRemainingSize() >= scnLen,
     "Not enough buffer space to serialize SCIONHeader" );

    // if( opts.FixLengths ){
    HdrLen = uint8_t(scnLen / LineLen);
    // PayloadLen = uint16_t(len(b.Bytes()) - scnLen);

    // Serialize common header.
    uint32_t firstLine =
        uint32_t(_version & 0xF) << 28 | (uint32_t(TrafficClass) << 20) | (FlowID & 0xFFFFF);
    start.WriteHtonU32(firstLine);
    start.WriteU8(_nextHdr);
    start.WriteU8(HdrLen);
    start.WriteHtonU16(PayloadLen);
    start.WriteU8(static_cast<uint8_t>(PathType)); // 8
    start.WriteU8((uint8_t(dstAddrType & 0xF) << 4) | uint8_t(srcAddrType & 0xF));
    start.WriteU16(0);   

	NS_ASSERT_MSG( iter_to_begin.distance_to( start ) == CmnHdrLen ,
	 "Buffer Iter Position should be CmHdrLen after serialization of CommonHeader");

    // Serialize address header.
    auto res = SerializeAddrHdr(start);
    if(res){start += *res; }

	auto actualCmnAddr = iter_to_begin.distance_to( start );
	auto expectedCmnAddr =  uint32_t( CmnHdrLen + AddrHdrLen() );
	NS_ASSERT_MSG( (actualCmnAddr == expectedCmnAddr),
	 "The ScionHeader size does not match expectations after serializing Common and Address Headers: was " << actualCmnAddr << " expected: "<< expectedCmnAddr );
    

    // Serialize path header.
    _path.Serialize(start);
    start += _path.Len();

	NS_ASSERT_MSG( iter_to_begin.distance_to( start ) == scnLen,
	 "Actual ScionHeader size diverges from computed size" );


	return start ;
}


TypeId
SCIONHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::SCIONHeader")
                            .SetParent<Header>()
                            .SetGroupName("Internet")
                            .AddConstructor<SCIONHeader>();
    return tid;
}

TypeId
SCIONHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
SCIONHeader::Print([[maybe_unused]] std::ostream& os) const
{
}

uint32_t
SCIONHeader::GetSerializedSize() const
{
    auto scnLen = CmnHdrLen + AddrHdrLen() + _path.Len();
    NS_ASSERT(scnLen < MaxHdrLen);
    //	return serrors.New("header length exceeds maximum","max", MaxHdrLen, "actual", scnLen)
    NS_ASSERT((scnLen % LineLen) == 0);
    // return serrors.New("header length is not an integer multiple of line length","actual",
    // scnLen)

    // if( opts.FixLengths ){
    HdrLen = uint8_t(scnLen / LineLen);
    return HdrLen * LineLen;
}

// DecodeFromBytes decodes the SCION layer. DecodeFromBytes resets the internal state of this layer
// to the state defined by the passed-in bytes. Slices in the SCION layer reference the passed-in
// data, so care should be taken to copy it first should later modification of data be required
// before the SCION layer is discarded.
/*
func (s *SCION) DecodeFromBytes(data []byte, df gopacket.DecodeFeedback) error {



    // Decode path header.
    var err error
    hdrBytes := int(s.HdrLen) * LineLen
    pathLen := hdrBytes - CmnHdrLen - addrHdrLen
    if pathLen < 0 {
        return serrors.New("invalid header, negative pathLen",
            "hdrBytes", hdrBytes, "addrHdrLen", addrHdrLen, "CmdHdrLen", CmnHdrLen)
    }
    if minLen := offset + pathLen; len(data) < minLen {
        df.SetTruncated()
        return serrors.New("provided buffer is too small", "expected", minLen, "actual", len(data))
    }

    s.Path, err = s.getPath(s.PathType)
    if err != nil {
        return err
    }

    err = s.Path.DecodeFromBytes(data[offset : offset+pathLen])
    if err != nil {
        return err
    }
    s.Contents = data[:hdrBytes]
    s.Payload = data[hdrBytes:]

    return nil
}
*/
SCIONAddress
SCIONHeader::SCIONSrcAddress() const
{

	Address hostAddr;
	if( AddrTypeConv(srcAddrType) == Ipv4Address::GetType() )
	{
		hostAddr = Ipv4Address::ConvertFrom( RawSrcAddr);
	}
	else if ( AddrTypeConv(srcAddrType) == Ipv6Address::GetType() )
	{
		hostAddr = Ipv6Address::ConvertFrom( RawSrcAddr );
	}

    SCIONAddress src(SrcIA, hostAddr);
	
    return src;
}

/*!
 \returns raw L3 Ipv4/6 Address without port
*/
Address
SCIONHeader::SrcAddress() const
{
    /*if( srcAddrType == AddrType_t::T4Ip )
    {
       InetSocketAddress src{ Ipv4Address::ConvertFrom(RawSrcAddr) };
       src.SetPort( )
       return
    }*/

    return RawSrcAddr;
}

/*!
 \returns raw L3 Ipv4/6 Address without port
*/
const Address&
SCIONHeader::DstAddress() const
{
    return RawDstAddr;
}

SCIONAddress
SCIONHeader::SCIONDstAddress() const
{
Address hostAddr;
	if( AddrTypeConv(dstAddrType) == Ipv4Address::GetType() )
	{
		hostAddr = Ipv4Address::ConvertFrom( RawDstAddr);
	}
	else if ( AddrTypeConv(srcAddrType) == Ipv6Address::GetType() )
	{
		hostAddr = Ipv6Address::ConvertFrom( RawDstAddr );
	}

    SCIONAddress dst(DstIA, hostAddr);
    return dst;
}

void
SCIONHeader::Serialize(Buffer::Iterator start) const
{
    SerializeOptions opts;

    Serialize( output_t{start}, opts );

    /*
	auto iter_to_begin = start; // only for debug assertions

    uint32_t scnLen = CmnHdrLen + AddrHdrLen() + _path.Len();
    NS_ASSERT_MSG(scnLen < MaxHdrLen,"The ScionHeader must not exceede a max of " << MaxHdrLen << " bytes");
    
    NS_ASSERT_MSG((scnLen % LineLen) == 0,"The ScionHeader length must be a multiple of 4 bytes but is: "<< scnLen);    

    NS_ASSERT_MSG( start.GetRemainingSize() >= scnLen, "Not enough buffer space to serialize SCIONHeader");

    // if( opts.FixLengths ){
    HdrLen = uint8_t(scnLen / LineLen);
    // PayloadLen = uint16_t(len(b.Bytes()) - scnLen);

    // Serialize common header.
    uint32_t firstLine =
        uint32_t(_version & 0xF) << 28 | (uint32_t(TrafficClass) << 20) | (FlowID & 0xFFFFF);
    start.WriteHtonU32(firstLine);
    start.WriteU8(_nextHdr);
    start.WriteU8(HdrLen);
    start.WriteHtonU16(PayloadLen);
    start.WriteU8(static_cast<uint8_t>(PathType)); // 8
    start.WriteU8((uint8_t(dstAddrType & 0xF) << 4) | uint8_t(srcAddrType & 0xF));
    start.WriteU16(0);   

	NS_ASSERT_MSG( start.GetDistanceFrom(iter_to_begin) == CmnHdrLen ,
	 "Buffer Iter Position should be CmHdrLen after serialization of CommonHeader");

    // Serialize address header.
    auto res = SerializeAddrHdr(start);
    if(res){ start += *res;}

	auto actualCmnAddr = start.GetDistanceFrom(iter_to_begin);
	auto expectedCmnAddr =  uint32_t( CmnHdrLen + AddrHdrLen() );
	NS_ASSERT_MSG( (actualCmnAddr == expectedCmnAddr),
	 "The ScionHeader size does not match expectations after serializing Common and Address Headers: was " << actualCmnAddr << " expected: "<< expectedCmnAddr );
    

    // Serialize path header.
    _path.Serialize(start);
    start += _path.Len();

	NS_ASSERT_MSG( start.GetDistanceFrom(iter_to_begin) == scnLen,
	 "Actual ScionHeader size diverges from computed size" );

     */
}

/* this is to remain compatibility with Add/Remove Header */
uint32_t
SCIONHeader::Deserialize( Buffer::Iterator start)
{
	auto tmp = Deserialize( input_t(start) ,nullptr);
    if( tmp)
    { return *tmp;
    }else
    {
        throw error_exception( tmp.error() );
    }
}


std::expected<uint32_t,error>
SCIONHeader::Deserialize( input_t start ,LayerStore* store)
{
    m_content = start;

    // Decode common header.

if( const auto rsize= start.GetRemainingSize() < CmnHdrLen )
{
        if(store)
		{store->set_truncated();}

		return std::unexpected( basic_error{"packet is shorter than the common header length",
			"min", std::to_string( CmnHdrLen),
             "actual", std::to_string( rsize ) } );
	}

    auto firstLine = start.ReadNtohU32(); // binary.BigEndian.Uint32(data[:4])
    _version = uint8_t(firstLine >> 28);
    TrafficClass = uint8_t((firstLine >> 20) & 0xFF);
    FlowID = firstLine & 0xFFFFF;

    _nextHdr = L4ProtocolType_t(start.ReadU8());
    HdrLen = start.ReadU8();
    PayloadLen = start.ReadNtohU16();                   // binary.BigEndian.Uint16(data[6:8])
    PathType = static_cast<pathType_t>(start.ReadU8()); // path.Type(data[8])
    auto byte9 = start.ReadU8();
    dstAddrType = AddrType_t(byte9 >> 4 & 0xF);
    srcAddrType = AddrType_t(byte9 & 0xF);
	start += 2; // skip reserved Uint16 zero bits
	NS_ASSERT_MSG( m_content.distance_to( start ) == CmnHdrLen , "CmnHeader size mismatch on Deserialization");
    

    // Decode address header.
   if( auto tmp = DecodeAddrHdr(start); tmp )
   {
    start = *tmp;
   } else
   {
    return std::unexpected( tmp.error() );
   }
	

// Decode path header.
    switch(PathType)
    {
 
    case pathType_t::DecodedPath:
    _path = DecodedPath();
    case pathType_t::OneHopPath:

    case pathType_t::EmptyPath:

    case pathType_t::RawPath:   // <= probably this should be used only
    default:
    _path =RawPath();
    break;
    };

	
	const uint32_t addrHdrB = AddrHdrLen();
	const uint32_t hdrBytes = HdrLen * LineLen;
	const uint32_t pathLen = hdrBytes - CmnHdrLen - addrHdrB;

	if ( pathLen < 0 )
    {
		return std::unexpected(
            basic_error{"invalid header, negative pathLen",
			"hdrBytes", std::to_string( hdrBytes ),
             "addrHdrLen", std::to_string( addrHdrB ),
              "CmdHdrLen", std::to_string( CmnHdrLen) }
               );
	}

    
	//if(  uint32_t minLen = addrHdrB + CmnHdrLen + pathLen; start.GetRemainingSize() < minLen ) // hier muss es size() sein, anstelle von GetRemainingSize()
    if(const auto szize = start.GetRemainingSize(); szize < pathLen ) 
    {
		if(store) {store->set_truncated();}

		return std::unexpected(
            basic_error{"provided buffer is too small",
             "expected", std::to_string( pathLen ),
              "actual", std::to_string( start.GetRemainingSize() )
               } );
	}

    if( auto tmp = _path.Deserialize(start); tmp )
    {
        start += *tmp;
    }else
    {
        return std::unexpected( tmp.error() );
    }

    m_payload = start; 
	// this iterator is advanced by HdrLen*LineLen bytes compared to m_content
	// this is where the NextDecoder() will take up

    //return GetSerializedSize();
	return m_content.distance_to( m_payload );
}

uint32_t
SCIONHeader::pseudoHeaderChecksum(int length, uint8_t protocol) const
{ /*if len(s.RawDstAddr) == 0 {
      return 0, serrors.New("destination address missing")
  }
  if len(s.RawSrcAddr) == 0 {
      return 0, serrors.New("source address missing")
  }*/

    uint32_t csum;
    uint8_t* srcIA;
    uint8_t* dstIA;
    srcIA = (uint8_t*)malloc(8);
    dstIA = (uint8_t*)malloc(8);
    BigEndian::PutUint64(srcIA, uint64_t(SrcIA));
    BigEndian::PutUint64(dstIA, uint64_t(DstIA));
    for (auto i = 0; i < 8; i += 2)
    {
        csum += uint32_t(srcIA[i]) << 8;
        csum += uint32_t(srcIA[i + 1]);
        csum += uint32_t(dstIA[i]) << 8;
        csum += uint32_t(dstIA[i + 1]);
    }
    free(srcIA);
    free(dstIA);

    uint8_t* src_addr_buf = (uint8_t*)malloc(RawSrcAddr.GetLength());
    uint8_t* dst_addr_buf = (uint8_t*)malloc(RawDstAddr.GetLength());

    RawSrcAddr.CopyTo(src_addr_buf);
    RawDstAddr.CopyTo(dst_addr_buf);

    // Address length is guaranteed to be a multiple of 2 by the protocol.
    for (auto i = 0; i < RawSrcAddr.GetLength(); i += 2)
    {
        csum += uint32_t(src_addr_buf[i]) << 8;
        csum += uint32_t(src_addr_buf[i + 1]);
    }
    for (auto i = 0; i < RawDstAddr.GetLength(); i += 2)
    {
        csum += uint32_t(dst_addr_buf[i]) << 8;
        csum += uint32_t(dst_addr_buf[i + 1]);
    }
    auto l = uint32_t(length);
    csum += (l >> 16) + (l & 0xffff);
    csum += uint32_t(protocol);

    free(src_addr_buf);
    free(dst_addr_buf);
    return csum;
}

std::expected<input_t, error>
SCIONHeader::DecodeAddrHdr( input_t start)
{
     if( start.GetRemainingSize() < AddrHdrLen() )
     {
    	return std::unexpected( basic_error{"provided buffer is too small to decode AddressHeader",
             "expected", std::to_string(AddrHdrLen()),
             "actual", std::to_string(start.GetRemainingSize() ) } );
    }

    DstIA = start.ReadNtohU64();
    SrcIA = start.ReadNtohU64();

    // the addressTypes of both Src and Dst have already been parsed at this point
    auto dstAddrBytes = AddrTypeLength(dstAddrType);
    auto srcAddrBytes = AddrTypeLength(srcAddrType);

    // RawDstAddr = Address(uint8_t type, const uint8_t* buffer, uint8_t len);
    // RawDstAddr = data[offset : offset+dstAddrBytes]
    uint8_t* tmp = (uint8_t*)malloc(dstAddrBytes);
    start.Read(tmp, dstAddrBytes);
    auto bytesRead = RawDstAddr.CopyFrom(tmp, dstAddrBytes);
    NS_ASSERT(bytesRead == uint32_t(dstAddrBytes));

    tmp = (uint8_t*)realloc((void*)tmp, srcAddrBytes);
    start.Read(tmp, srcAddrBytes);
    bytesRead = RawSrcAddr.CopyFrom(tmp, srcAddrBytes);
    NS_ASSERT(bytesRead == uint32_t(srcAddrBytes));
    // RawSrcAddr = data[offset : offset+srcAddrBytes]

    free(tmp);
    return start;
}

void
SCIONHeader::SetDstAddress(const SCIONAddress& dst)
{
    DstIA = dst.GetIA();
    RawDstAddr = dst.GetHostAddress();
	dstAddrType = ConvAddrType(RawDstAddr.GetType());
	NS_ASSERT( (dstAddrType == AddrType_t::T4Ip) || (dstAddrType == AddrType_t::T16Ip) );
}

void
SCIONHeader::SetSrcAddress(const Address& src)
{
    RawSrcAddr = src;
    srcAddrType = ConvAddrType(RawSrcAddr.GetType());
}

void
SCIONHeader::SetDstAddress(const Address& dst)
{
    RawDstAddr = dst;
    dstAddrType = ConvAddrType(dst.GetType());
}

void
SCIONHeader::SetSrcAddress(const SCIONAddress& src)
{
    SrcIA = src.GetIA();
    RawSrcAddr = src.GetHostAddress();
    srcAddrType = ConvAddrType(RawSrcAddr.GetType());
}

// this error return value is unneccessary !! 
std::expected<size_t, error>
SCIONHeader::SerializeAddrHdr( buffer_iterator start) const
{
    const auto expected = static_cast<std::ptrdiff_t>(2*sizeof(IA_t) + RawDstAddr.GetLength() + RawSrcAddr.GetLength() );
    NS_ASSERT_MSG( static_cast<std::ptrdiff_t>(start.GetRemainingSize() ) >= expected, "Not enough buffer space to serialize AddressHeader" );
	auto iter_begin = start;

    start.WriteHtonU64(DstIA);
    start.WriteHtonU64(SrcIA);

	NS_ASSERT_MSG( AddrTypeConv(dstAddrType) == RawDstAddr.GetType() ,
	 "logic error expected: " << std::to_string(dstAddrType) << " conv: "
	  << std::to_string( AddrTypeConv(dstAddrType) ) 
	  << " actual: " << std::to_string( RawDstAddr.GetType() ) );

	NS_ASSERT_MSG( AddrTypeConv(srcAddrType) == RawSrcAddr.GetType() , "logic error" );

	NS_ASSERT_MSG( AddrTypeLength(dstAddrType) == RawDstAddr.GetLength(),
	 "Expected: " << AddrTypeLength(dstAddrType)<< " got: " << std::to_string( RawDstAddr.GetLength() ) );

	uint8_t* buffer =(uint8_t*) malloc( AddrTypeLength(dstAddrType) );
	RawDstAddr.CopyTo(buffer);
	start.Write(buffer,RawDstAddr.GetLength( )) ;
    // WriteTo(start, RawDstAddr);

//    WriteTo(start, RawSrcAddr);

    NS_ASSERT( AddrTypeLength(srcAddrType) == RawSrcAddr.GetLength() );
	uint8_t* buffer2 =(uint8_t*) malloc( AddrTypeLength(srcAddrType) );
	RawSrcAddr.CopyTo(buffer2);
	start.Write(buffer2,RawSrcAddr.GetLength( )) ;
	free(buffer);
	free(buffer2);

	// WriteTo does NOT serialize the Addresses type, but only the internal mac array
	// (writes Address::GetLength() bytes not Address::GetSerializedSize() )
    
    auto actual = iter_begin.distance_to( start );
    NS_ASSERT_MSG( actual == expected
    ,"size mismatch in AddressHeader: actual:" << std::to_string(actual)
    << " expected: " << std::to_string( expected ) 
    );

    return iter_begin.distance_to( start );
}

} // namespace ns3
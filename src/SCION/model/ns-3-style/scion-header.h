#pragma once

#include "ns3/gopacket++.h"
#include "ns3/header.h"
#include "ns3/scion-path.h"
#include "ns3/address.h"
#include "ns3/isd-as.h"
#include "ns3/gopacket++.h"
#include "ns3/scion-types.h"
#include "ns3/go-errors.h"
#include <expected>


namespace ns3 
{

class SCIONAddress;

// this could be made a static method of SCIONHeader
std::expected<input_t,error> decodeSCION(input_t data,LayerStore* pb );


// SCION is the header of a SCION packet.
class SCIONHeader : public Header ,public virtual Layer
{
public:
	~SCIONHeader(){};
	SCIONHeader()=default;

  /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;

    void Print(std::ostream& os) const override;
    uint32_t GetSerializedSize() const override;

    void Serialize(Buffer::Iterator start) const override;
    std::expected<uint32_t,error> Deserialize( input_t start,LayerStore* = nullptr) override;
	uint32_t Deserialize( Buffer::Iterator start) override;


	virtual output_t Serialize( output_t start, SerializeOptions ) const override;

	SCIONAddress SCIONSrcAddress() const;
	SCIONAddress SCIONDstAddress() const;

	Address SrcAddress() const;
	const Address& DstAddress() const;

	void SetSrcAddress( const SCIONAddress&);
	void SetDstAddress( const SCIONAddress&);
	void SetSrcAddress( const Address& );
	void SetDstAddress( const Address& );

	pathType_t GetPathType()const{return PathType; }
	const Path& GetPath(){ return _path;}
	void SetPath(const Path&p ){ _path = p; }
	void SetPath( Path&& p){ _path = std::move(p); }

	IA_t GetSrcIA()const {return SrcIA; }
	IA_t GetDstIA() const {return DstIA; }
	void SetSrcIA( IA_t ia ){ SrcIA = ia; }
	void SetDstIA( IA_t ia ){ DstIA = ia; }

	uint8_t Version() const {return _version; }
	L4ProtocolType_t NextHeader()const {return _nextHdr;}
	void SetNextHeader( L4ProtocolType_t type ){ _nextHdr = type; }

	void SetPayloadLength( uint16_t paylen){ PayloadLen = paylen;}
	uint16_t GetPayloadLength()const{ return PayloadLen; }

	void SetVersion( uint8_t v){_version = v;}
	uint8_t GetVersion()const{return _version;}

	void SetTrafficClass( uint8_t tc ){ TrafficClass = tc;}
	uint8_t GetTrafficClass()const{ return TrafficClass;}

	AddrType_t GetDstAddrType() const {return dstAddrType;}
	AddrType_t GetSrcAddrType() const {return srcAddrType;}
    

	static LayerType staticLayerType(){return m_type;}
private:

	// Layer interface
	input_t Contents()const override{return m_content;};
	input_t Payload()const override{return m_payload; };

	LayerType Type() const override ;
	std::optional<LayerType> NextType()const override;

	LayerClass CanDecode() const override;

	input_t m_content;
	input_t m_payload;

	//------- Common Header fields--------------------

	// Version is version of the SCION Header. Currently, only 0 is supported.
	uint8_t _version = 0;
	// TrafficClass denotes the traffic class. Its value in a received packet or fragment might be
	// different from the value sent by the packet’s source. The current use of the Traffic Class
	// field for Differentiated Services and Explicit Congestion Notification is specified in
	// RFC2474 and RFC3168
	uint8_t TrafficClass;
	// FlowID is a 20-bit field used by a source to label sequences of packets to be treated in the
	// network as a single flow. It is mandatory to be set.
	uint32_t FlowID ;
	// NextHdr  encodes the type of the first header after the SCION header.
	// This can be either a SCION extension or a layer-4 protocol such as TCP or UDP.
	// Values of this field respect and
	// extend IANA’s assigned internet protocol numbers.
	L4ProtocolType_t _nextHdr ;
	// HdrLen is the length of the SCION header in multiples of 4 bytes.
	// The SCION header length is computed as HdrLen * 4 bytes.
	// The 8 bits of the HdrLen field limit the SCION header to a
	// maximum of 255 * 4 == 1020 bytes.
	mutable uint8_t HdrLen ; //(mutable because it is computed in serialize() which is const )
	// PayloadLen is the length of the payload in bytes. The payload includes extension headers and
	// the L4 payload. This field is 16 bits long, supporting a maximum payload size of 64KB.
	uint16_t PayloadLen =0;
	// PathType specifies the type of path in this SCION header.
	pathType_t PathType ;
	// DstAddrType (4 bit) is the type/length of the destination address.
	AddrType_t dstAddrType ;
	// SrcAddrType (4 bit) is the type/length of the source address.
	AddrType_t srcAddrType ;

	// Address header fields.

	// DstIA is the destination ISD-AS.
	IA_t DstIA;
	// SrcIA is the source ISD-AS.
	IA_t SrcIA;
	// RawDstAddr is the destination address.
    Address	RawDstAddr;
	// RawSrcAddr is the source address.
	Address RawSrcAddr;

	// Path is the path contained in the SCION header. It depends on the PathType field.
 	Path _path;


	inline static LayerType m_type =  LayerTypeRegistry::instance().RegisterLayerType(
	LayerTypeMetaData{
			.m_name =    "SCION",
			.m_decoder= &decodeSCION
		}
	);

//	pathPool    []path.Path
	// pathPoolRaw path.Path

// AddrHdrLen returns the length of the address header (destination and source ISD-AS-Host triples)
// in bytes.
uint8_t AddrHdrLen() const{
	 return 2*IABYTES + AddrTypeLength( dstAddrType ) + AddrTypeLength( srcAddrType );
	// return 2*IABYTES + RawDstAddr.GetLength() + RawSrcAddr.GetLength();
}

// SerializeAddrHdr serializes destination and source ISD-AS-Host address triples into the provided
// buffer. The caller must ensure that the correct address types and lengths are set in the SCION
// layer, otherwise the results of this method are undefined.
std::expected<size_t,error> SerializeAddrHdr( buffer_iterator start ) const;


// DecodeAddrHdr decodes the destination and source ISD-AS-Host address triples from the provided
// buffer. The caller must ensure that the correct address types and lengths are set in the SCION
// layer, otherwise the results of this method are undefined.
std::expected<input_t,error> DecodeAddrHdr( input_t start );

uint32_t pseudoHeaderChecksum(int length,uint8_t protocol )const;

};

LayerType scionNextLayerTypeL4( L4ProtocolType_t t);
LayerType scionNextLayerType( L4ProtocolType_t t);




}  // ns3 namespace 
/*


// RecyclePaths enables recycling of paths used for DecodeFromBytes. This is
// only useful if the layer itself is reused.
// When this is enabled, the Path instance may be overwritten in
// DecodeFromBytes. No references to Path should be kept in use between
// invocations of DecodeFromBytes.
func (s *SCION) RecyclePaths() {
	if s.pathPool == nil {
		s.pathPool = []path.Path{
			empty.PathType:  empty.Path{},
			onehop.PathType: &onehop.Path{},
			scion.PathType:  &scion.Raw{},
			epic.PathType:   &epic.Path{},
		}
		s.pathPoolRaw = path.NewRawPath()
	}
}

// getPath returns a new or recycled path for pathType
func (s *SCION) getPath(pathType path.Type) (path.Path, error) {
	if s.pathPool == nil {
		return path.NewPath(pathType)
	}
	if int(pathType) < len(s.pathPool) {
		return s.pathPool[pathType], nil
	}
	return s.pathPoolRaw, nil
}




func ParseAddr(addrType AddrType, raw []byte) (addr.Host, error) {
	switch addrType {
	case T4Ip:
		var raw4 [4]byte
		copy(raw4[:], raw)
		return addr.HostIP(netip.AddrFrom4(raw4)), nil
	case T4Svc:
		svc := addr.SVC(binary.BigEndian.Uint16(raw[:2]))
		return addr.HostSVC(svc), nil
	case T16Ip:
		var raw16 [16]byte
		copy(raw16[:], raw)
		return addr.HostIP(netip.AddrFrom16(raw16)), nil
	}
	return addr.Host{}, serrors.New("unsupported address type/length combination",
		"type", addrType, "len", addrType.Length())
}

func PackAddr(host addr.Host) (AddrType, []byte, error) {
	switch host.Type() {
	case addr.HostTypeIP:
		ip := host.IP()
		if !ip.IsValid() {
			break
		}
		t := T4Ip
		if ip.Is6() {
			t = T16Ip
		}
		return t, ip.AsSlice(), nil
	case addr.HostTypeSVC:
		raw := make([]byte, 4)
		binary.BigEndian.PutUint16(raw, uint16(host.SVC()))
		return T4Svc, raw, nil
	}
	return 0, nil, serrors.New("unsupported address", "addr", host)
}





// computeChecksum computes the checksum with the SCION pseudo header.
func (s *SCION) computeChecksum(upperLayer []byte, protocol uint8) (uint16, error) {
	if s == nil {
		return 0, serrors.New("SCION header missing")
	}
	csum, err := s.pseudoHeaderChecksum(len(upperLayer), protocol)
	if err != nil {
		return 0, err
	}
	csum = s.upperLayerChecksum(upperLayer, csum)
	folded := s.foldChecksum(csum)
	return folded, nil
}

func (s *SCION) upperLayerChecksum(upperLayer []byte, csum uint32) uint32 {
	// Compute safe boundary to ensure we do not access out of bounds.
	// Odd lengths are handled at the end.
	safeBoundary := len(upperLayer) - 1
	for i := 0; i < safeBoundary; i += 2 {
		csum += uint32(upperLayer[i]) << 8
		csum += uint32(upperLayer[i+1])
	}
	if len(upperLayer)%2 == 1 {
		csum += uint32(upperLayer[safeBoundary]) << 8
	}
	return csum
}

func (s *SCION) foldChecksum(csum uint32) uint16 {
	for csum > 0xffff {
		csum = (csum >> 16) + (csum & 0xffff)
	}
	return ^uint16(csum)
}
*/
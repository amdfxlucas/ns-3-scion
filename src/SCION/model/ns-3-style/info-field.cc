// #include "/usr/include/c++/13/format"
#include <format>
#include "ns3/info-field.h"

#include "ns3/endian-utils.h"
#include "ns3/nstime.h"

#include <sstream>

namespace ns3
{

// SerializeTo writes the fields into the provided buffer.
// The buffer must be of length >= path.InfoLen.
// @ requires  len(b) >= InfoLen
// SerializeTo modifies the contents of b and reads (but does not modify) the fields of inf.
// @ preserves acc(b) && acc(inf, 1/2)
// When a call that satisfies the precondition (len(b) >= InfoLen) is made,
// the return value is guaranteed to be nil.
// @ ensures   err == nil
// SerializeTo always terminates.
// @ decreases

/*
void
InfoField::Serialize(Buffer::Iterator start) const
{
   Serialize( buffer_iterator{start} );
}
*/
InfoField::operator std::string() const
{
    std::stringstream ss;
    ss << Time::FromInteger(Timestamp, ns3::Time::Unit::S);
    return std::format("[Peer: {0}, ConsDir: {1}, SegID: {2}, Timestamp: {3} ]",
                       Peer,
                       ConsDir,
                       SegID,
                       ss.str());

    // return {};
}

// DecodeFromBytes populates the fields from a raw buffer.
// The buffer must be of length >= path.InfoLen.
// @ requires  len(raw) >= InfoLen
// DecodeFromBytes modifies *inf and reads (but does not modify) the contents of raw.
// @ preserves acc(inf) && acc(raw, 1/2)
// When a call that satisfies the precondition (len(raw) >= InfoLen) is made,
// the return value is guaranteed to be nil.
// @ ensures   err == nil
// DecodeFromBytes always terminates.
// @ decreases
/*
uint32_t
InfoField::Deserialize(Buffer::Iterator start)
{
return Deserialize( const_buffer_iterator{start} );
} */

uint32_t
InfoField::Deserialize(const_buffer_iterator start)
{
    auto fstByte = start.ReadU8();
    ConsDir = (fstByte & 0x1) == 0x1;
    Peer = (fstByte & 0x2) == 0x2;

    start.ReadU8(); // Reserved

    SegID = start.ReadNtohU16();
    Timestamp = start.ReadNtohU32();

    return INFO_FIELD_LEN;
}

void
InfoField::Serialize( buffer_iterator start) const
{
    NS_ASSERT_MSG( start.GetRemainingSize() >= Len(), "Not enough buffer space to serialize InfoField" );

    uint8_t fstByte = 0;
    if (ConsDir)
    {
        fstByte |= 0x1;
    }
    if (Peer)
    {
        fstByte |= 0x2;
    }
    start.WriteU8(fstByte);
    start.WriteU8(0); // reserved
    start.WriteHtonU16(SegID);
    start.WriteHtonU32(Timestamp);
}

// UpdateSegID updates the SegID field by XORing the SegID field with the 2
// first bytes of the MAC. It is the beta calculation according to
// https://docs.scion.org/en/latest/protocols/scion-header.html#hop-field-mac-computation
//
//	UpdateSegID only accesses and modifies the contents of inf.SegID.
//
// @ preserves acc(&inf.SegID)
// UpdateSegID always terminates.
// @ decreases
void
InfoField::UpdateSegID(uint8_t hfMac[MACLEN])
{
    //@ share hfMac
    SegID = SegID ^ BigEndian::Uint16(hfMac);
}

} // namespace ns3
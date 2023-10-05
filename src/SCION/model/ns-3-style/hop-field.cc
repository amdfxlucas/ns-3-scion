#include "ns3/hop-field.h"
#include <format>
namespace ns3
{
// DecodeFromBytes populates the fields from a raw buffer.
// The buffer must be of length >= path.HopLen.
// @ requires  len(raw) >= HopLen
// DecodeFromBytes modifies the fields of *h and reads (but does not modify) the contents of raw.
// @ preserves acc(h) && acc(raw, 1/2)
// When a call that satisfies the precondition (len(raw) >= HopLen) is made,
// the return value is guaranteed to be nil.
// @ ensures   err == nil
// Calls to DecodeFromBytes are always guaranteed to terminate.
// @ decreases


// SerializeTo writes the fields into the provided buffer.
// The buffer must be of length >= path.HopLen.
// @ requires  len(b) >= HopLen
// SerializeTo reads (but does not modify) the fields of *h and writes to the contents of b.
// @ preserves acc(h, 1/2) && acc(b)
// When a call that satisfies the precondition (len(b) >= HopLen) is made,
// the return value is guaranteed to be nil.
// @ ensures   err == nil
// Calls to SerializeTo are guaranteed to terminate.
// @ decreases


HopField::operator std::string() const
{
 return std::format( "[EgressRouterAlert: {}, IngressRouterAlert: {}, ExpTime: {} , ConsIngress: {}, ConsEgress: {} ]"
                ,EgressRouterAlert, IngressRouterAlert, ExpTime, ConsIngress, ConsEgress ) ;
}

uint32_t
HopField::Deserialize( const_buffer_iterator start)
{
    auto fstByte = start.ReadU8();
    EgressRouterAlert = (fstByte & 0x1 )== 0x1;
    IngressRouterAlert =( fstByte & 0x2 )== 0x2;
    ExpTime = start.ReadU8();
    ConsIngress = start.ReadNtohU16();
    ConsEgress = start.ReadNtohU16();

    start.Read(Mac, MACLEN);
    return HOPLEN;
}

void
HopField::Serialize( buffer_iterator start) const
{
        NS_ASSERT_MSG( start.GetRemainingSize() >= Len(),
         "Not enough buffer space to serialize HopField" );

  uint8_t fstByte = 0;
    if (EgressRouterAlert)
    {
            fstByte |= 0x1;
    }
    if (IngressRouterAlert )
    {
            fstByte |= 0x2;
    }
    start.WriteU8(fstByte);
    start.WriteU8(ExpTime);
    start.WriteHtonU16(ConsIngress);
    start.WriteHtonU16(ConsEgress);
    start.Write(Mac, MACLEN);
}

}
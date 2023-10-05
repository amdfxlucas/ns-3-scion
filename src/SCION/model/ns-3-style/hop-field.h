#pragma once
#include <stdint.h>
#include "ns3/fields-forward.h"
#include "ns3/buffer.h"
#include "ns3/iterator-util.h"

namespace ns3
{
/*!
	\brief  HopField is the HopField used in the SCION and OneHop path types.

 The Hop Field has the following format:

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|r r r r r r I E|    ExpTime    |           ConsIngress         |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|        ConsEgress             |                               |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
	|                              MAC                              |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
struct HopField 
 {
	// IngressRouterAlert flag. If the IngressRouterAlert is set, the ingress router (in
	// construction direction) will process the L4 payload in the packet.
	bool IngressRouterAlert = false;
	// EgressRouterAlert flag. If the EgressRouterAlert is set, the egress router (in
	// construction direction) will process the L4 payload in the packet.
	bool EgressRouterAlert = false;
	// Exptime is the expiry time of a HopField. The field is 1-byte long, thus there are 256
	// different values available to express an expiration time. The expiration time expressed by
	// the value of this field is relative, and an absolute expiration time in seconds is computed
	// in combination with the timestamp field (from the corresponding info field) as follows
	//
	// Timestamp + (1 + ExpTime) * (24*60*60)/256
	uint8_t ExpTime;
	// ConsIngress is the ingress interface ID in construction direction.
	uint16_t ConsIngress;
	// ConsEgress is the egress interface ID in construction direction.
	uint16_t ConsEgress;
	// Mac is the 6-byte Message Authentication Code to authenticate the HopField.
	uint8_t Mac [MACLEN];


    void Serialize( buffer_iterator start ) const;
    uint32_t Deserialize( const_buffer_iterator start );
	
	constexpr static uint8_t Len() {return HOPLEN; }// in bytes (in serialized form)

	auto operator<=>(const HopField& other )const = default;
	friend std::ostream& operator<< (std::ostream& out, const HopField& hop);
	operator std::string() const;
};

inline std::ostream& operator<< (std::ostream& out, const HopField& hop)
{
 return out << static_cast<std::string>( hop ) << std::endl;
}


// ExpTimeToDuration calculates the relative expiration time in seconds.
// Note that for a 0 value ExpTime, the minimal duration is expTimeUnit.
// ExpTimeToDuration is pure: it does not modify any memory locations and
// does not produce any side effects.
// @ pure
// Calls to ExpTimeToDuration are guaranteed to always terminate.
// @ decreases
//func ExpTimeToDuration( uint8_t expTime ) time.Duration {
//	return (time.Duration(expTime) + 1) * time.Duration(expTimeUnit) * time.Second
// }

}// namespace ns3
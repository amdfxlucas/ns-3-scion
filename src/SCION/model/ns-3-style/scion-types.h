#pragma once
#include <stdint.h>

#define ENDHOST_PORT 30041

#define AS_FROM_IA( ia ) ( ((uint64_t)ia<<16) >>16  )// ( (uint64_t)ia & 0xffffffffffff ) 
// get last 48bit of 64 bit IA that correspond to the AS
#define ISD_FROM_IA( ia ) (ia >> 48 )
 // get first 16 bit of 64 bit IA that correspond to ISD
#define _MAKE_IA_(isd, as) ((((uint64_t)isd) << 48) | ((uint64_t)as))


	// LineLen is the length of a SCION header line in bytes.
	#define LineLen  4
	// CmnHdrLen is the length of the SCION common header in bytes.
	#define CmnHdrLen  12
	// MaxHdrLen is the maximum allowed length of a SCION header in bytes.
	#define MaxHdrLen  1020
	// SCIONVersion is the currently supported version of the SCION header format. Different
	// versions are not guaranteed to be compatible to each other.
	#define SCIONVersion  0

namespace ns3
{
// IA represents the ISD (ISolation Domain) and AS (Autonomous System) Id of a given SCION AS.
// The highest 16 bit form the ISD number and the lower 48 bits form the AS number.
using  IA_t = uint64_t;
using AS_t = uint64_t;
using ISD_t = uint16_t;

enum L4ProtocolType_t : uint8_t
{

	L4None  = 0,
	L4TCP   = 6,
	L4UDP   = 17,
	L4SCMP  = 202,
	L4BFD   = 203,
	HopByHopClass  = 200,
	End2EndClass  = 201
};

// AddrType indicates the type of a host address in the SCION header.
// The AddrType consists of a sub-type and length part, both two bits wide.
// The four possible lengths are 4B (0), 8B (1), 12B (2), or 16B (3) bytes.
// There are four possible sub-types per address length.


// AddrType constants
enum AddrType_t : uint8_t
{
	T4Ip  = 0b0000, // T=0, L=0
	T4Svc          = 0b0100, // T=1, L=0
	T16Ip          = 0b0011 // T=0, L=3
};
// Length returns the length of this AddrType value.
constexpr inline int AddrTypeLength( AddrType_t a) 
{
	return LineLen * (1 + ( a & 0x3));
}

static_assert( AddrTypeLength( T4Ip) == 4);
static_assert( AddrTypeLength( T4Svc) == 4);
static_assert( AddrTypeLength( T16Ip) ==16);

	struct GeoCoordinates
	{
		// Latitude of the geographic coordinate, in the WGS 84 datum.
		float _latitude;
		// Longitude of the geographic coordinate, in the WGS 84 datum.
		float _longitude;
		// GeoCoordinates describes a geographical position (of a border router on the path).
		std::string _civicAddress;
	};

	// an interface ID of a local net device
	using IFIDx_t = uint32_t;

	// an ingress or egress Interface of an AS
	using ASIFID_t = uint64_t; // deprecated with version 2 of scion header

	struct PathInterface
	{
		IA_t _ia;
		ASIFID_t _id;
		auto operator <=>(const PathInterface&) const = default;
	};

	using PathHash_t = std::string;


	enum LinkType_t : uint8_t
	{
		// LinkTypeUnset represents an unspecified link type.
		LinkTypeUnset,
		// LinkTypeDirect represents a direct physical connection.
		LinkTypeDirect,
		// LinkTypeMultihop represents a connection with local routing/switching.
		LinkTypeMultiHop,
		// LinkTypeOpennet represents a connection overlayed over publicly routed Internet
		LinkTypeOpenNet
	};

}
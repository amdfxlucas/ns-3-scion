#pragma once
#include <stdint.h>
#include <string>
#include "ns3/scion-types.h"
#include "ns3/scion-path.h"
namespace ns3
{

enum : uint16_t
{
    SvcDS = 0x0001,
    SvcCS = 0x0002,
    SvcWildcard = 0x0010,
    SvcNone = 0xffff,

    SVCMcast = 0x8000,
};

// SVC is a SCION service address.
// A service address is a short identifier for the service type, and a
// flag-bit for multicast.
// The package's SVC constant values are defined without multicast.
// The Multicast and Base methods set/unset the multicast flag.
// using SVC_t = uint16_t;

struct SVC_t
{
    SVC_t(std::string);
    // IsMulticast returns the value of the multicast flag.
    bool IsMulticast() const;
    // Base returns the SVC identifier with the multicast flag unset.
    SVC_t Base() const;
    // Multicast returns the SVC identifier with the multicast flag set.
    SVC_t Multicast() const;
    operator std::string() const;

    // BaseString returns the upper case name of the service.
    // For unrecognized services, it
    // returns "<SVC:Hex-Value>".
    std::string BaseString() const;

  private:
    uint16_t _svc;
};

class SVCAddress
{
    // ParseSVC returns the SVC address corresponding to str. For anycast
    // SVC addresses, use CS_A and DS_A; shorthand versions without
    // the _A suffix (e.g., CS) also return anycast SVC addresses. For multicast,
    // use CS_M, and DS_M.
    SVCAddress(const std::string&);

    // IsMulticast returns the value of the multicast flag.
    bool IsMulticast() const;
    // Base returns the SVC identifier with the multicast flag unset.
    SVC_t Base() const;
    // Multicast returns the SVC identifier with the multicast flag set.
    SVC_t Multicast() const;

    Path GetPath() const
    {
        return _path;
    }

  private:
    SVC_t _svc;
    IA_t _ia;
    Path _path;
    Address _nextHop; // L4Address (address:port pair)
};

} // namespace ns3
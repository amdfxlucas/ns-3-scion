#pragma once
#include "ns3/address.h"
#include "ns3/scion-path.h"
#include "ns3/scion-snet-path-meta-data.h"

namespace ns3
{
/*
TODO:

maybe create an UDPAddress (or better L4Address ) that stores either
 an InetSocketAddress or an Inet6SocketAddress internally
 Represents the idea of an address Port Pair
*/

// model for type erasure
class SNETPathConcept
{
  public:
    // UnderlayNextHop returns the address:port pair of a local-AS underlay
    // speaker. Usually, this is a border router that will forward the traffic.
    virtual const Address& UnderlayNextHop() const = 0; // maybe return UDPAddress

    // Dataplane returns a path that should be used in the dataplane. The
    // underlying dataplane path object is returned directly without any copy.
    // If you modify the raw data, you must ensure that there are no data races
    // or data corruption on your own.
    virtual Path& DataplanePath() const = 0;

    // Source is the AS the path starts from. Empty paths return the local
    // AS of the router that created them.
    virtual IA_t SrcIA() const = 0;
    // Destination is the AS the path points to. Empty paths return the local
    // AS of the router that created them.
    virtual IA_t DstIA() const = 0;
    // Metadata returns supplementary information about this path.
    // Returns nil if the metadata is not available.
    virtual PathMetaData& Metadata() = 0;
};

/*
TODO: change interface to allow null values to be returned 
from UnderlayNextHop, DataplanePath() and MetaData() -> return std::shared_ptr
*/
class SNETPath
{
  public:
    // UnderlayNextHop returns the address:port pair of a local-AS underlay
    // speaker. Usually, this is a border router that will forward the traffic.
    const Address& UnderlayNextHop() const
    {
        return _nextHop;
    } // maybe return L4Address

	void SetUnderlayNextHop( const Address& a){ _nextHop = a; }

    // Dataplane returns a path that should be used in the dataplane. The
    // underlying dataplane path object is returned directly without any copy.
    // If you modify the raw data, you must ensure that there are no data races
    // or data corruption on your own.
    const Path& DataplanePath() const
    {
        return _path;
    }
	void SetDataPlanePath( const Path& p ){_path = p; }

    // Source is the AS the path starts from. Empty paths return the local
    // AS of the router that created them.
    IA_t SrcIA() const
    {
        return _srcIA;
    }

    // Destination is the AS the path points to. Empty paths return the local
    // AS of the router that created them.
    IA_t DstIA() const
    {
        return _dstIA;
    }

    // Metadata returns supplementary information about this path.
    // Returns nil if the metadata is not available.
    const PathMetaData& Metadata() const
    {
        return _meta;
    }

	// reverses the DataplanePath, as well as Src & Dst IA
	void Reverse()
	{
		std::swap( _srcIA,_dstIA);
		_path.Reverse();
	}

    std::string Fingerprint() const
    {
        return _meta.Fingerprint();
    }

    bool IsInterfaceOnPath(const PathInterface& pi) const
    {
        return _meta.IsInterfaceOnPath(pi);
    };

  private:
    IA_t _srcIA;
    IA_t _dstIA;
    Address _nextHop; //  a address:port pair (maybe L4Addr here )
    Path _path;
    PathMetaData _meta;
};

} // namespace ns3

namespace std
{
template <>
struct hash<ns3::SNETPath>
{
    auto operator()(const ns3::SNETPath& p) const -> size_t
    {
        //  return hash<SNETPath>{}(xyz.value);
        return std::hash<std::string>{}(p.Metadata().Fingerprint());
    }
};
} // namespace std
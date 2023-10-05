#pragma once
#include <vector>
#include <string>
#include <stdint.h>

#include "ns3/scion-types.h"
#include "ns3/nstime.h"

namespace ns3
{
	

	// EpicAuths is a container for the EPIC hop authenticators.
	struct EPICAuth
	{ /*
		// AuthPHVF is the authenticator for the penultimate hop.
		 Buffer _authPHVF;
		// AuthLHVF is the authenticator for the last hop
	 	Buffer _authLHVF;
		bool supportsEPIC() const
		{
			return _authPHVF.size() == 16 && _authLHVF.size() == 16;
		}
		*/
	};




	/*
	 TODO :

	 add pan.PathMetaData methods/fields here -> unify both into one


struct pathHop
{
	PathInterface _a;
	PathInterface _b;
};

type pathHopSet map[pathHop]struct{}

func (a pathHopSet) subsetOf(b pathHopSet) bool {
	for x := range a {
		if _, inB := b[x]; !inB {
			return false
		}
	}
	return true
}


	// NOTE: a similar method exists in snet/path/path.go:  func fmtInterfaces(ifaces []snet.PathInterface) []string 
	 func (pm *PathMetadata) fmtInterfaces() string   // needed for pan.Path::String()


	 // LowerLatency compares the latency of two paths.
	// Returns
	//   - true, true if a has strictly lower latency than b
	//   - false, true if a has equal or higher latency than b
	//   - _, false if not enough information is available to compare a and b
	func (pm *PathMetadata) LowerLatency(b *PathMetadata) (bool, bool) 

	// latencySum returns the total latency and the set of edges with unknown
	// latency
	// NOTE: the latency from the end hosts to the first/last interface is always
	// unknown. If that would be taken into account, all the paths become
	// incomparable.
	func (pm *PathMetadata) latencySum() (time.Duration, pathHopSet) 

// HigherBandwidth compares the bandwidth of two paths.
// Returns
//   - true, true if a has strictly higher bandwidth than b
//   - false, true if a has equal or lower bandwidth than b
//   - _, false if not enough information is available to compare a and b
func (pm *PathMetadata) HigherBandwidth(b *PathMetadata) (bool, bool) 


// bandwidthMin returns the min (bottleneck) bandwidth and the set of edges
// with unknown bandwidth.
func (pm *PathMetadata) bandwidthMin() (uint64, pathHopSet) {



	*/

	// PathMetadata contains supplementary information about a path.
	//
	// The information about MTU, Latency, Bandwidth etc. are based solely on data
	// contained in the AS entries in the path construction beacons. These entries
	// are signed/verified based on the control plane PKI. However, the
	// *correctness* of this meta data has *not* been checked.
	class PathMetaData
	{
		public:
		// returns a sha256 hash
		PathHash_t Fingerprint() const;
		bool IsInterfaceOnPath(const PathInterface &pi) const;

	private:
		// Interfaces is a list of interfaces on the path.
		std::vector<PathInterface> _interfaces;

		// MTU is the maximum transmission unit for the path, in bytes.
		uint16_t _mtu;

		// Expiry is the expiration time of the path.
		Time _expiry;

		// Latency lists the latencies between any two consecutive interfaces.
		// Entry i describes the latency between interface i and i+1.
		// Consequently, there are N-1 entries for N interfaces.
		// A negative value (LatencyUnset) indicates that the AS did not announce a
		// latency for this hop.
		std::vector< Time> _latencies; // this is a Duration not a time !!

		// Bandwidth lists the bandwidth between any two consecutive interfaces, in Kbit/s.
		// Entry i describes the bandwidth between interfaces i and i+1.
		// A 0-value indicates that the AS did not announce a bandwidth for this hop.
		std::vector<uint64_t> _bandwidths;

		// Geo lists the geographical position of the border routers along the path.
		// Entry i describes the position of the router for interface i.
		// A 0-value indicates that the AS did not announce a position for this router.
		std::vector<GeoCoordinates> _coordinates;

		// LinkType contains the announced link type of inter-domain links.
		// Entry i describes the link between interfaces 2*i and 2*i+1.
		std::vector<LinkType_t> _linkType;

		// InternalHops lists the number of AS internal hops for the ASes on path.
		// Entry i describes the hop between interfaces 2*i+1 and 2*i+2 in the same AS.
		// Consequently, there are no entries for the first and last ASes, as these
		// are not traversed completely by the path.
		std::vector<uint32_t> _internalHops;

		// Notes contains the notes added by ASes on the path, in the order of occurrence.
		// Entry i is the note of AS i on the path.
		std::vector<std::string> _notes;

		// EpicAuths contains the EPIC authenticators.
		EPICAuth _epicAuths;
	};
}
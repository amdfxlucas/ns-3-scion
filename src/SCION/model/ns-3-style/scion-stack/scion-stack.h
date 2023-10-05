#pragma once

#include "ns3/object.h"
#include "ns3/scion-types.h"
#include "ns3/l4-address.h"
#include "ns3/scion-types.h"
#include <map>
#include <vector>

namespace ns3
{
struct PathReqFlags 
 {
	bool Refresh ;
	bool Hidden ;
};

// ASInfo provides information about the local AS.
struct ASInfo 
{
	IA_t IA  ;
	uint16_t MTU ;
};

    /*
        the scion stack is like the HostContext in Go.
        It can be aggregated to a Node, to make it SCIONCapable
        (with Object::AggregateObject(Ptr<Object> other)  )

        All objects that are (potentially) shared between multiple Sockets on the same node,
        i.e. PathPool , PathStatsDB etc live here
    */
    class SCIONContext : public Object // this already  incorporates  the deamon (it is not modelled in this simulation )
    {
    public:
		SCIONContext(ISD_t isd, IA_t ia, AS_t as) : m_isd(isd), m_as(as), m_ia(ia){}
        IA_t LocalIA() const{return m_ia;}
        AS_t LocalAS() const{return m_as;}
        ISD_t LocalISD() const{return m_isd;}


    /*func (h *hostContext) queryPaths(ctx context.Context, dst IA) ([]*Path, error) 
	flags := daemon.PathReqFlags{Refresh: false, Hidden: false}
	snetPaths, err := h.sciond.Paths(ctx, addr.IA(dst), 0, flags)
    return snetPaths; */

	// Paths requests from the daemon a set of end to end paths between the source and destination.
	// Paths( dst, src addr.IA, f PathReqFlags) ([]snet.Path, error)

	// ASInfo requests from the daemon information about AS ia, the zero IA can be
	// use to detect the local IA.
	// ASInfo ASInfo( IA_t ia );

	// IFInfo requests from SCION Daemon addresses and ports of interfaces. Slice
	// ifs contains interface IDs of BRs. If empty, a fresh (i.e., uncached)
	// answer containing all interfaces is returned.
	// IFInfo(  ifs []common.IFIDType) (map[common.IFIDType]*net.UDPAddr, error)

    std::map< ASIFID_t, L4Address > IFInfo( const  std::vector< ASIFID_t> ifs ) const{return {}; }

	// SVCInfo requests from the daemon information about addresses and ports of
	// infrastructure services.  Slice svcTypes contains a list of desired
	// service types. If unset, a fresh (i.e., uncached) answer containing all
	// service types is returned. The reply is a map from service type to URI of
	// the service.
	// SVCInfo( svcTypes []addr.HostSVC) (map[addr.HostSVC]string, error)

	// RevNotification sends a RevocationInfo message to the daemon.
	// RevNotification(  revInfo *path_mgmt.RevInfo) error
	// Close shuts down the connection to the daemon.
	void Close(){};
    private:
	ISD_t m_isd;
	AS_t m_as;
	IA_t m_ia;
     std::map< ASIFID_t, L4Address > m_border_router_addresses;

    };
}
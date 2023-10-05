#pragma once
#include "ns3/application.h"
#include "ns3/scion-types.h"
#include "ns3/net-device.h"
#include "ns3/scion-udp-socket-impl.h"
#include "ns3/l4-address.h"
#include "ns3/packet-socket.h"
namespace ns3
{

class ScionAs;


/*

what is the difference between interfaces and external ?!

type DataPlane struct {
    
	interfaces        map[uint16]BatchConn   // interfaces[0] == internal

	external          map[uint16]BatchConn  // inter AS connections for the given AS interface ID
                                            // but they are also stored in interfaces under the same id

	linkTypes         map[uint16]topology.LinkType

	neighborIAs       map[uint16]addr.IA

	peerInterfaces    map[uint16]uint16

	internal          BatchConn

	internalIP        netip.Addr

	internalNextHops  map[uint16]*net.UDPAddr
	svc               *services
	macFactory        func() hash.Hash
	bfdSessions       map[uint16]bfdSession
	localIA           addr.IA
	mtx               sync.Mutex
	running           bool
	Metrics           *Metrics
	forwardingMetrics map[uint16]forwardingMetrics
}
*/



/*
does the border router really not need to know the neighbouring relations ?!
*/
class BorderRouterApplication;
class BorderRouterRoutingState : public Object
{
    friend class BorderRouterApplication;
public:
/*!
   maps AS interface IDs to local interface IDs of this Node,
   via which it is connected to that AS
   \param local_if ID of an interface local to this Node.
   \param as_if ID of the AS interface through wich this Node is connected via its Interface local_if
*/
    // std::map< AS_t, IFIDx_t> m_as_to_local_device;

    /* given we want to reach AS with id X,
     we can go to any of  {[k, v],[]..}= m_connectivity[x] 's  of our local device interfaces k,
     and thus arrive at X's remote interface v
     */
    struct connectivity_t // oder besser sowas wie remote oder out- connectivity
    {
        std::map< AS_t , std::set< std::pair< IFIDx_t, ASIFID_t> >  > m_conn;

        std::map<AS_t/*remote*/, std::map< ASIFID_t /*remoteIFID*/, IFIDx_t /*localDevice ID*/> >  m_conn_out;

        void insert( AS_t as, IFIDx_t local_idx, ASIFID_t remote_as_ifid );
    };
    connectivity_t m_connectivity;

    std::map< ASIFID_t, IFIDx_t > m_local; // maps local AS Interface IDs to local NetDevice IDs
    // das sind die echten IfIDs hier, die in GetNode()->GetDevice(idx) benutzt werden können !!

    /* this might seem  duplicate with ScionAS's GetRemoteASInfo() but its not.
        the AS only has the global view with granularity of  (ASIFID_t) AS interfaces.

    */

   std::map< std::pair< AS_t/*remoteAS*/, ASIFID_t /*remoteASIFid*/>,
                                         IFIDx_t/*localDevice*/ > m_proper_local;
    // ASIFID_t are ambigous without the AS number

    std::map< IFIDx_t /*localDeviceIDx*/, AS_t /*remoteAS*/ > m_inv_local;

   
   // all devices below this index must be IP devices
    IFIDx_t m_fstNonIP_IFIDx; // this is set by the first BorderRouterApplication that is constructed

    // here we listen for packets from endHosts 
    Ptr<SCIONUdpSocketImpl> m_sock; // THIS wont work !! 
    // SCIONUdpSockets return only the payload after the scion header was stripped off ( but we need the header here)
    // We need a RAW_SCION_Socket here !!

    L4Address m_listenAddr;
    std::map< IFIDx_t , Ptr<PacketSocket> > m_inter_domain_sockets;



IA_t GetLocalIA()const{return m_localIA; }

    AS_t m_localAS;
    IA_t m_localIA;
};

class BorderRouterApplication : public Application
{
public:
    void StartApplication();
    void StopApplication();
    BorderRouterApplication();
    BorderRouterApplication(std::pair<ScionAs*,ASIFID_t> from,
                            std::pair<ScionAs*,ASIFID_t> to, 
                            Ptr<NetDevice> device, IFIDx_t peerDeviceID );

// void DeliverPacketLocally( Ptr<Packet> packet );
//void ProcessReceivedPacket( uint16_t local_if, ScionPacket* packet, Time receive_time) override;


    void OnRecvFromOtherBr(Ptr<Socket>);

    void OnRecvFromEndHost(Ptr<Socket>);



private:
void DetermineAndSetLocalListenAddress();

    
/*
 /// Traced Callback: received packets, source address.
    TracedCallback<Ptr<const Packet>, const Address&> m_rxEndHostTrace;
*/


    bool is_intra_domain_listener = false; // only the first borderRouterApplication on this node will listen on the UDPSocket for endhosts



    Ptr<PacketSocket> m_inter_sock;
    /* reicht hier ein socket aus , oder brauche ich für jedes device eines ?!
     Es braucht tatsächlich für jeden link-endpoint ein netDevice mit dazugehörigem söckchen 
     das Ist aber bei dem gewählten design nicht schlimm: 
     für jedes IFID_t (also netDevice das mit einem anderen BorderRouter verbunden ist) gibt es
     eine eigene nur dafür zuständige BRApplication instanz, die nur dieses söckchen bewacht.
     Aber wie soll dann das forwarden an ein anderes device des BR aussehen ?
      dazu bräuchte ich ja das söckchen einer anderen application instanz...
      Oder kann ich einfach direkt mit NetDevice::Send(packet, dest,protocol ) in das device schreiben 
      ohne (Packet)Socket ?!
     */



};

}
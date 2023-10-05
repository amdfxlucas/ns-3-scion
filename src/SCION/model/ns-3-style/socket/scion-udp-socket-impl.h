/*
 * Copyright (c) 2007 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#ifndef SCION_UDP_SOCKET_IMPL_H
#define SCION_UDP_SOCKET_IMPL_H
//#include "ns3/core-module.h"
#include "ns3/icmpv4.h"
#include "ns3/ipv4-interface.h"
#include "ns3/udp-socket.h"

#include "ns3/callback.h"
#include "ns3/ipv4-address.h"
#include "ns3/ptr.h"
#include "ns3/socket.h"
#include "ns3/traced-callback.h"

#include "ns3/scion-types.h"
#include "ns3/scion-path-policy.h"

#include <queue>
#include <stdint.h>
#include "ns3/udp-header.h"
#include "ns3/scion-header.h"
#include "ns3/scion-snet-path.h"
#include "ns3/scion-path-selector.h"
#include "ns3/scion-reply-selector.h"
#include "ns3/l4-address.h"

#include <memory>

namespace ns3
{
class Ipv4EndPoint;
class Ipv6EndPoint;
class Node;
class Packet;
class UdpL4Protocol;
class Ipv6Header;
class Ipv6Interface;


class Path;

class SCIONHeader;

/*

// HostAddrType discriminates between different types of Host addresses.
type HostAddrType uint8

const (
	HostTypeNone HostAddrType = iota
	HostTypeIP
	HostTypeSVC
)

func (t HostAddrType) String() string {
	switch t {
	case HostTypeNone:
		return "None"
	case HostTypeIP:
		return "IP"
	case HostTypeSVC:
		return "SVC"
	}
	return fmt.Sprintf("UNKNOWN (%d)", t)
}

// Host represents the AS-local host identifier of a SCION address.
//
// Different address types (IPv4, IPv6, SVC) are all represented with
// this Host struct, discriminated with a Type() field.
//
// The zero value is a valid object with Host{}.Type() == HostTypeNone.
type Host struct {
	ip  netip.Addr
	svc SVC
	t   HostAddrType
}

// Addr is a full SCION address, composed of ISD, AS and Host part.
type addr.Addr struct {
	IA   IA
	Host Host
}
*/

/*// DataplanePath is an abstract representation of a SCION dataplane path.
type DataplanePath interface {
	// SetPath sets the path in the SCION header. It assumes that all the fields
	// except the path and path type are set correctly.
	SetPath(scion *slayers.SCION) error
}
*/

/*
class DataplanePath // un neccessary
{
public:
void SetFromHeader( SCIONHeader& ); 
// the naming of Go is contra intuitive 
// (this method writes the path into the header(it is an out param))
};
*/

/*
normal udp address, but also has a SCION Reply Path
// this is needed by the impl in snet (which is not followed here)
*/
class SCIONUDPAddress
{/*snet.UDPAddr
	IA      addr.IA
	Path    DataplanePath
	NextHop *net.UDPAddr  // IP,port, zone   also eine InetAddress oder Inet6Address
	Host    *net.UDPAddr*/
public:
//private:
  IA_t _ia;
//  DataplanePath _path; // this is from snet
  Path _path;
  Address _nextHop;
  Address _host; // eine Inet oder Inet6SocketAddress
  // enthält auch den srcPort des inneren udp headers
  // uin16_t _port;

// ia, und host können in einer normalen SCIONAddress zusammeng

};

/*
  in case of ReceiveFrom

  _ia is the senders IA
  _nextHop is the  outer UDP layers sender address 
  _host is the SCIONHeaders SrcsAddress field
*/

class SCIONUdpSocketImpl : public UdpSocket
{
  public:

/*
// ReplyPather creates reply paths based on the incoming RawPath.
type ReplyPather interface {
	// ReplyPath takes the RawPath of an incoming packet and creates a path
	// that can be used in a reply.
	ReplyPath(RawPath) (DataplanePath, error)
}
*/
  class SCIONReplyPather
  {
  public:
    //DataplanePath ComputeReplyPath( Path& );
    Path ComputeReplyPath( Path& pp ){ Path p{pp};  p.Reverse(); return p; }

  };

  class SCMPHandler
  {
    public:
    // in its implementation in Go, 
    // this handler is only ever interested in the SCMP Payload field
    // any outer Headers can already be removed from packet if passed in here
    int HandleSCMP(Ptr<Packet>& ){ return false;} // indicates SCMP ERROR CODE
  };
    struct DecodedPacket
    {
      Ptr<Packet> _packet;
      SCIONHeader _scion;
      UdpHeader _udp; // inner udp header
      // bool hasSCMPPayload
      // std::vector< std::shared_ptr< Header > > _headers; // instead ?!
    };

  class SCMPInterceptor
  {
    public:
    SCMPInterceptor() = default;
    /* interceptor needs to read from "us" the raw real UDP/IP socket */
    SCMPInterceptor( SCIONUdpSocketImpl * );
    void SetSCMPHandler(SCMPHandler* s){ m_scmp= s;}
    /*
      corresponds to (c *SCIONPacketConn) ReadFrom(pkt *Packet, ov *net.UDPAddr) error 
    */
      DecodedPacket ReadFrom( Ptr<Packet>& pkt, Address& addr, uint32_t maxSize, uint32_t flags );
    private:



  std::pair<DecodedPacket,bool> DoReadFrom( Ptr<Packet>& pkt, Address& addr, uint32_t maxSize, uint32_t flags );
    /* corresponds to func (c *SCIONPacketConn) readFrom(pkt *Packet, ov *net.UDPAddr) error */
     // void DoReadFrom( Ptr<Packet>& pkt, Address& addr );
     // maybe this signature should change to std::tuple< SCIONHeader, UdpHeader> DoReadFrom()
     // or std::vector< std::shared_ptr< Header > > 
     // in the go implementation, this methods decodes the packet and checks if it has SCMP payload

    SCIONUdpSocketImpl* m_socket = nullptr;  
    SCMPHandler* m_scmp = nullptr;
  };

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    /**
     * Create an unbound udp socket.
     */
    SCIONUdpSocketImpl();
    ~SCIONUdpSocketImpl() override;

    /**
     * \brief Set the associated node.
     * \param node the node
     */
    void SetNode(Ptr<Node> node);
    /**
     * \brief Set the associated UDP L4 protocol.
     * \param udp the UDP L4 protocol
     */
    void SetUdp(Ptr<UdpL4Protocol> udp);

    SocketErrno GetErrno() const override;
    SocketType GetSocketType() const override;
    Ptr<Node> GetNode() const override;

    int Bind() override;
    int Bind6() override;
    int Bind(const Address& address) override;

    int Close() override;

    int ShutdownSend() override;
    int ShutdownRecv() override;

    int Connect(const Address& address) override;
    int Listen() override;

    uint32_t GetTxAvailable() const override;

  // returns -1 on error else size-of the sent-packet
    int Send(Ptr<Packet> p, uint32_t flags) override;
    int SendTo(Ptr<Packet> p, uint32_t flags, const Address& address) override;

    uint32_t GetRxAvailable() const override;

    Ptr<Packet> Recv(uint32_t maxSize, uint32_t flags) override;
    Ptr<Packet> RecvFrom(uint32_t maxSize, uint32_t flags, Address& fromAddress) override;

    int GetSockName(Address& address) const override;
    int GetPeerName(Address& address) const override;

    int MulticastJoinGroup(uint32_t interfaceIndex, const Address& groupAddress) override;
    int MulticastLeaveGroup(uint32_t interfaceIndex, const Address& groupAddress) override;
    
    void BindToNetDevice(Ptr<NetDevice> netdevice) override;
    bool SetAllowBroadcast(bool allowBroadcast) override;
    bool GetAllowBroadcast() const override;
    void Ipv6JoinGroup(Ipv6Address address,
                       Socket::Ipv6MulticastFilterMode filterMode,
                       std::vector<Ipv6Address> sourceAddresses) override;

  private:
  SCMPInterceptor m_scmp_inter;
  // SCIONReplyPather m_reply;
  // for dialing -----
  SCIONPathSelector m_pathSelector;
  PathPolicy m_policy;
  // PathRefreshSubscriber m_refresh;
  // for listening ------
  SCIONDefaultReplySelector m_replySelector;
  SCIONAddress m_local; // set on bind()

// corresponds to func (c *SCIONPacketConn) WriteTo(pkt *Packet, ov *net.UDPAddr) error 
// there the packet was finally serialized and send through the underlying net.UDPConn
// (no longer needed here)
//    void WriteTo( Ptr<Packet> pkt, uint32_t flags, Address& addr );


    Address LocalListenAddress()const; // without port
    L4Address LocalSocketListenAddress()const; // with port
    // Address FillSCIONHeaderFromAddr( SCIONHeader& scion, SCIONUDPAddress& add );

    // ----------- dialed conn ------------------------------------------------------

// int Write( byte[]) --> int Send(Ptr<Packet> )
// int Read(byte[])  --> int Recv( Ptr<Packet> )
    // ------------listen conn ----------------------------------------------------
Ptr<Packet> RecvFromVia(uint32_t maxSize, uint32_t flags, Address& fromAddress );
 int SendToVia(Ptr<Packet> p, uint32_t flags, const Address& dst, std::shared_ptr<SNETPath> pathToDst );
    //--------------------------------------------------------------------------------

    int writeMsg( const SCIONAddress& src, const SCIONAddress& dst,
                  std::shared_ptr< SNETPath> path, uint32_t flags,Ptr<Packet> p );

    std::tuple<SCIONAddress, SNETPath > readMsg( Ptr<Packet>& p, Address& fromAddr, uint32_t maxSize, uint32_t flags);

  int PConnWriteTo( Ptr<Packet> p, uint32_t flags, const Address& toAddr);

  // low level impl from UDPSocketImpl ( everything with a UDP prefix )------------------------
    Ptr<Packet> DoRecvFromUDP( uint32_t maxSize, uint32_t flags, Address& fromAddress );

    int DoSendUDP( Ptr<Packet> p);
    int DoSendToUdp(Ptr<Packet> p, Ipv6Address dest, uint16_t port);
    int DoSendToUDP(Ptr<Packet> p, Ipv4Address dest, uint16_t port, uint8_t tos);

//---------------------------------------------------------------------------------------------
    // Attributes set through UdpSocket base class
    void SetRcvBufSize(uint32_t size) override;
    uint32_t GetRcvBufSize() const override;
    void SetIpMulticastTtl(uint8_t ipTtl) override;
    uint8_t GetIpMulticastTtl() const override;
    void SetIpMulticastIf(int32_t ipIf) override;
    int32_t GetIpMulticastIf() const override;
    void SetIpMulticastLoop(bool loop) override;
    bool GetIpMulticastLoop() const override;
    void SetMtuDiscover(bool discover) override;
    bool GetMtuDiscover() const override;

    /**
     * \brief UdpSocketFactory friend class.
     * \relates UdpSocketFactory
     */
    friend class UdpSocketFactory;
    // invoked by Udp class

    /**
     * Finish the binding process
     * \returns 0 on success, -1 on failure
     */
    int FinishBind();

    /**
     * \brief Called by the L3 protocol when it received a packet to pass on to TCP.
     *
     * \param packet the incoming packet
     * \param header the packet's IPv4 header
     * \param port the remote port
     * \param incomingInterface the incoming interface
     */
    void ForwardUp(Ptr<Packet> packet,
                   Ipv4Header header,
                   uint16_t port,
                   Ptr<Ipv4Interface> incomingInterface);

    /**
     * \brief Called by the L3 protocol when it received a packet to pass on to TCP.
     *
     * \param packet the incoming packet
     * \param header the packet's IPv6 header
     * \param port the remote port
     * \param incomingInterface the incoming interface
     */
    void ForwardUp6(Ptr<Packet> packet,
                    Ipv6Header header,
                    uint16_t port,
                    Ptr<Ipv6Interface> incomingInterface);

    /**
     * \brief Kill this socket by zeroing its attributes (IPv4)
     *
     * This is a callback function configured to m_endpoint in
     * SetupCallback(), invoked when the endpoint is destroyed.
     */
    void Destroy();

    /**
     * \brief Kill this socket by zeroing its attributes (IPv6)
     *
     * This is a callback function configured to m_endpoint in
     * SetupCallback(), invoked when the endpoint is destroyed.
     */
    void Destroy6();

    /**
     * \brief Deallocate m_endPoint and m_endPoint6
     */
    void DeallocateEndPoint();

    /**
     * \brief Send a packet
     * \param p packet
     * \returns 0 on success, -1 on failure
     */
    int DoSend(Ptr<Packet> p);
    /**
     * \brief Send a packet to a specific destination and port (IPv4)
     * \param p packet
     * \param daddr destination address
     * \param dport destination port
     * \param tos ToS
     * \returns 0 on success, -1 on failure
     */
    int DoSendTo(Ptr<Packet> p, Ipv4Address daddr, uint16_t dport, uint8_t tos);
    /**
     * \brief Send a packet to a specific destination and port (IPv6)
     * \param p packet
     * \param daddr destination address
     * \param dport destination port
     * \returns 0 on success, -1 on failure
     */
    int DoSendTo(Ptr<Packet> p, Ipv6Address daddr, uint16_t dport);

    /**
     * \brief Called by the L3 protocol when it received an ICMP packet to pass on to TCP.
     *
     * \param icmpSource the ICMP source address
     * \param icmpTtl the ICMP Time to Live
     * \param icmpType the ICMP Type
     * \param icmpCode the ICMP Code
     * \param icmpInfo the ICMP Info
     */
    void ForwardIcmp(Ipv4Address icmpSource,
                     uint8_t icmpTtl,
                     uint8_t icmpType,
                     uint8_t icmpCode,
                     uint32_t icmpInfo);

    /**
     * \brief Called by the L3 protocol when it received an ICMPv6 packet to pass on to TCP.
     *
     * \param icmpSource the ICMP source address
     * \param icmpTtl the ICMP Time to Live
     * \param icmpType the ICMP Type
     * \param icmpCode the ICMP Code
     * \param icmpInfo the ICMP Info
     */
    void ForwardIcmp6(Ipv6Address icmpSource,
                      uint8_t icmpTtl,
                      uint8_t icmpType,
                      uint8_t icmpCode,
                      uint32_t icmpInfo);

    // Connections to other layers of TCP/IP
    Ipv4EndPoint* m_endPoint;  //!< the IPv4 endpoint
    Ipv6EndPoint* m_endPoint6; //!< the IPv6 endpoint
    Ptr<Node> m_node;          //!< the associated node
    Ptr<UdpL4Protocol> m_udp;  //!< the associated UDP L4 protocol
    Callback<void, Ipv4Address, uint8_t, uint8_t, uint8_t, uint32_t>
        m_icmpCallback; //!< ICMP callback
    Callback<void, Ipv6Address, uint8_t, uint8_t, uint8_t, uint32_t>
        m_icmpCallback6; //!< ICMPv6 callback

    Address m_defaultAddress;                      //!< Default address
    uint16_t m_defaultPort;                        //!< Default port
    TracedCallback<Ptr<const Packet>> m_dropTrace; //!< Trace for dropped packets

    mutable SocketErrno m_errno; //!< Socket error code
    bool m_shutdownSend;         //!< Send no longer allowed
    bool m_shutdownRecv;         //!< Receive no longer allowed
    bool m_connected;            //!< Connection established
    bool m_allowBroadcast;       //!< Allow send broadcast packets

    std::queue<std::pair<Ptr<Packet>, Address>> m_deliveryQueue; //!< Queue for incoming packets
    uint32_t m_rxAvailable; //!< Number of available bytes to be received

    // Socket attributes
    uint32_t m_rcvBufSize;    //!< Receive buffer size
    uint8_t m_ipMulticastTtl; //!< Multicast TTL
    int32_t m_ipMulticastIf;  //!< Multicast Interface
    bool m_ipMulticastLoop;   //!< Allow multicast loop
    bool m_mtuDiscover;       //!< Allow MTU discovery


};

} // namespace ns3

#endif /* SCION_UDP_SOCKET_IMPL_H */

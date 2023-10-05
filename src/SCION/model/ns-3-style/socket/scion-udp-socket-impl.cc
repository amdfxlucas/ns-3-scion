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

#include "scion-udp-socket-impl.h"

#include "ns3/ipv4-end-point.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv4-route.h"
#include "ns3/ipv4-packet-info-tag.h"
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/ipv4.h"
#include "ns3/ipv6-end-point.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/ipv6-packet-info-tag.h"
#include "ns3/ipv6-route.h"
#include "ns3/ipv6-routing-protocol.h"
#include "ns3/ipv6.h"
#include "ns3/udp-l4-protocol.h"
#include "ns3/l4-address.h"

#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/trace-source-accessor.h"

#include "ns3/scion-stack.h"
#include "ns3/scion-default-path-selector.h"

#include "ns3/udp-header.h"
#include "ns3/scion-header.h"
#include "ns3/scion-address.h"

#include <limits>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SCIONUdpSocketImpl");

NS_OBJECT_ENSURE_REGISTERED(SCIONUdpSocketImpl);

// The correct maximum UDP message size is 65507, as determined by the following formula:
// 0xffff - (sizeof(IP Header) + sizeof(UDP Header)) = 65535-(20+8) = 65507
// \todo MAX_IPV4_UDP_DATAGRAM_SIZE is correct only for IPv4
static const inline uint32_t MAX_IPV4_UDP_DATAGRAM_SIZE = 65507; //!< Maximum UDP datagram size

// Add attributes generic to all UdpSockets to base class UdpSocket
TypeId
SCIONUdpSocketImpl::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::SCIONUdpSocketImpl")
            .SetParent<UdpSocket>()
            .SetGroupName("Internet")
            .AddConstructor<SCIONUdpSocketImpl>()
            .AddTraceSource("Drop",
                            "Drop UDP packet due to receive buffer overflow",
                            MakeTraceSourceAccessor(&SCIONUdpSocketImpl::m_dropTrace),
                            "ns3::Packet::TracedCallback")
            .AddAttribute("IcmpCallback",
                          "Callback invoked whenever an icmp error is received on this socket.",
                          CallbackValue(),
                          MakeCallbackAccessor(&SCIONUdpSocketImpl::m_icmpCallback),
                          MakeCallbackChecker())
            .AddAttribute("IcmpCallback6",
                          "Callback invoked whenever an icmpv6 error is received on this socket.",
                          CallbackValue(),
                          MakeCallbackAccessor(&SCIONUdpSocketImpl::m_icmpCallback6),
                          MakeCallbackChecker());
    return tid;
}

SCIONUdpSocketImpl::SCIONUdpSocketImpl()
    : m_endPoint(nullptr),
      m_endPoint6(nullptr),
      m_node(nullptr),
      m_udp(nullptr),
      m_errno(ERROR_NOTERROR),
      m_shutdownSend(false),
      m_shutdownRecv(false),
      m_connected(false),
      m_rxAvailable(0)
{
    NS_LOG_FUNCTION(this);
    m_allowBroadcast = false;
}

SCIONUdpSocketImpl::~SCIONUdpSocketImpl()
{
    NS_LOG_FUNCTION(this);

    /// \todo  leave any multicast groups that have been joined
    m_node = nullptr;
    /**
     * Note: actually this function is called AFTER
     * SCIONUdpSocketImpl::Destroy or SCIONUdpSocketImpl::Destroy6
     * so the code below is unnecessary in normal operations
     */
    if (m_endPoint != nullptr)
    {
        NS_ASSERT(m_udp);
        /**
         * Note that this piece of code is a bit tricky:
         * when DeAllocate is called, it will call into
         * Ipv4EndPointDemux::Deallocate which triggers
         * a delete of the associated endPoint which triggers
         * in turn a call to the method SCIONUdpSocketImpl::Destroy below
         * will will zero the m_endPoint field.
         */
        NS_ASSERT(m_endPoint != nullptr);
        m_udp->DeAllocate(m_endPoint);
        NS_ASSERT(m_endPoint == nullptr);
    }
    if (m_endPoint6 != nullptr)
    {
        NS_ASSERT(m_udp);
        /**
         * Note that this piece of code is a bit tricky:
         * when DeAllocate is called, it will call into
         * Ipv4EndPointDemux::Deallocate which triggers
         * a delete of the associated endPoint which triggers
         * in turn a call to the method SCIONUdpSocketImpl::Destroy below
         * will will zero the m_endPoint field.
         */
        NS_ASSERT(m_endPoint6 != nullptr);
        m_udp->DeAllocate(m_endPoint6);
        NS_ASSERT(m_endPoint6 == nullptr);
    }
    m_udp = nullptr;
}


int
SCIONUdpSocketImpl::FinishBind()
{
    m_local = SCIONAddress{GetNode()->GetObject<SCIONContext>()->LocalIA(), LocalSocketListenAddress() };

    // the socket wants to get notified about important events by the EndPoint
    NS_LOG_FUNCTION(this);
    bool done = false;
    if (m_endPoint != nullptr)
    {
        m_endPoint->SetRxCallback(
            MakeCallback(&SCIONUdpSocketImpl::ForwardUp, Ptr<SCIONUdpSocketImpl>(this)));
        m_endPoint->SetIcmpCallback(
            MakeCallback(&SCIONUdpSocketImpl::ForwardIcmp, Ptr<SCIONUdpSocketImpl>(this)));
        m_endPoint->SetDestroyCallback(
            MakeCallback(&SCIONUdpSocketImpl::Destroy, Ptr<SCIONUdpSocketImpl>(this)));
        done = true;
    }
    if (m_endPoint6 != nullptr)
    {
        m_endPoint6->SetRxCallback(
            MakeCallback(&SCIONUdpSocketImpl::ForwardUp6, Ptr<SCIONUdpSocketImpl>(this)));
        m_endPoint6->SetIcmpCallback(
            MakeCallback(&SCIONUdpSocketImpl::ForwardIcmp6, Ptr<SCIONUdpSocketImpl>(this)));
        m_endPoint6->SetDestroyCallback(
            MakeCallback(&SCIONUdpSocketImpl::Destroy6, Ptr<SCIONUdpSocketImpl>(this)));
        done = true;
    }
    if (done)
    {
        m_shutdownRecv = false;
        m_shutdownSend = false;
        return 0;
    }
    return -1;
}

int
SCIONUdpSocketImpl::Close()
{   // for listen
    // stats.unsubscribe( m_replySelector)
    // m_replySelector.Close()

    // for dial
    // m_refresh.Close()
    // m_pathSelector.Close()


    // -----------------------------
    NS_LOG_FUNCTION(this);
    if (m_shutdownRecv && m_shutdownSend)
    {
        m_errno = Socket::ERROR_BADF;
        return -1;
    }
    Ipv6LeaveGroup();
    m_shutdownRecv = true;
    m_shutdownSend = true;
    DeallocateEndPoint();
    return 0;
}

/*! 
  \param  address SCIONAddress to remote
*/
int
SCIONUdpSocketImpl::Connect(const Address& address)
{
    NS_LOG_FUNCTION(this << address);
 /*   if (InetSocketAddress::IsMatchingType(address))
    {
        InetSocketAddress transport = InetSocketAddress::ConvertFrom(address);
        m_defaultAddress = Address(transport.GetIpv4());
        m_defaultPort = transport.GetPort();
        SetIpTos(transport.GetTos());
        m_connected = true;
        NotifyConnectionSucceeded();
    }
    else if (Inet6SocketAddress::IsMatchingType(address))
    {
        Inet6SocketAddress transport = Inet6SocketAddress::ConvertFrom(address);
        m_defaultAddress = Address(transport.GetIpv6());
        m_defaultPort = transport.GetPort();
        m_connected = true;
        NotifyConnectionSucceeded();
    }
    else*/ if ( SCIONAddress::IsMatchingType(address ) )
    {
        SCIONAddress transport = SCIONAddress::ConvertFrom(address);
        m_defaultAddress = address;
        m_defaultPort = transport.GetPort();
        m_connected = true;

        // instantiate PathSelector & PathRefresherSubscriber

        if( transport.GetIA() != GetNode()->GetObject<SCIONContext>()->LocalIA() )
        {
            m_pathSelector = SCIONDefaultPathSelector();
            // policy, m_refresher
        }


        NotifyConnectionSucceeded();

    }
    else
    {
        NotifyConnectionFailed();
        return -1;
    }

    return 0;
}

int
SCIONUdpSocketImpl::Send(Ptr<Packet> p, uint32_t flags)
{
    NS_LOG_FUNCTION(this << p << flags);

    if (!m_connected)
    { // Connect() wasnt called
        m_errno = ERROR_NOTCONN;
        return -1;
    }
    // connect() was called with normal Ip4/6 address, not scion
    if (Ipv4Address::IsMatchingType(m_defaultAddress) ||
        Ipv6Address::IsMatchingType(m_defaultAddress))
    {
     //   return DoSend(p);
     NS_ASSERT(false);// this is not intended usage , i changed my mind
    }

    else // SocketIs Connected to ScionAddress
    {
        SCIONAddress remote =
            SCIONAddress::ConvertFrom(m_defaultAddress); // scion addresses already contain a port
             SCIONAddress local{GetNode()->GetObject<SCIONContext>()->LocalIA(), LocalSocketListenAddress()}; // with port

        std::shared_ptr< SNETPath > path = nullptr;
        
        if (local.GetIA() != SCIONAddress::ConvertFrom(m_defaultAddress).GetIA())
        {
            path =std::make_shared<SNETPath>( m_pathSelector.Path() );
           
           
        }
         return writeMsg(local, remote, path, flags, p);
    }
}

int
SCIONUdpSocketImpl::DoSend(Ptr<Packet> p)
{
  return DoSendUDP(p);
}

int
SCIONUdpSocketImpl::DoSendTo(Ptr<Packet> p, Ipv4Address dest, uint16_t port, uint8_t tos)
{
   return DoSendToUDP(p,dest,port,tos);
}

int
SCIONUdpSocketImpl::DoSendTo(Ptr<Packet> p, Ipv6Address dest, uint16_t port)
{
return DoSendToUdp( p, dest, port );
}


// can only be called after Socker was Bound
Address SCIONUdpSocketImpl::LocalListenAddress()const
{
    bool isIpv6 = m_endPoint6 != nullptr;
    bool isIpv4 = m_endPoint != nullptr;
    NS_ASSERT( !isIpv4 != !isIpv6); // XOR (only one of them can be true)

    auto listenAddr = isIpv6 ?  m_endPoint6->GetLocalAddress().ConvertTo() 
                             :  m_endPoint->GetLocalAddress().ConvertTo();
    return listenAddr;
}

// can only be called after Socker was Bound
L4Address SCIONUdpSocketImpl::LocalSocketListenAddress()const
{
    bool isIpv6 = m_endPoint6 != nullptr;
    bool isIpv4 = m_endPoint != nullptr;
    NS_ASSERT( !isIpv4 != !isIpv6); // XOR (only one of them can be true)

    auto listenAddr = isIpv6 ? Inet6SocketAddress{ m_endPoint6->GetLocalAddress(), m_endPoint6->GetLocalPort()}.ConvertTo() :
                              InetSocketAddress{ m_endPoint->GetLocalAddress(), m_endPoint->GetLocalPort() }.ConvertTo();
    return listenAddr;
}

int
SCIONUdpSocketImpl::writeMsg(const SCIONAddress& src,
                             const SCIONAddress& dst,
                             std::shared_ptr< SNETPath> path,
                             uint32_t flags,
                             Ptr<Packet> p)
{
    SCIONHeader scion;

    L4Address nextHop; // either an Inet or Inet6SocketAddress
    if( src.GetIA() == dst.GetIA() )
    {
        nextHop = SetPort( dst.GetHostAddress() , ENDHOST_PORT );
    } else
    {
        NS_ASSERT( path != nullptr );
        nextHop = path->UnderlayNextHop();
        scion.SetPath( path->DataplanePath() );
    }

    
    scion.SetDstIA( dst.GetIA() );
    scion.SetDstAddress( dst.GetHostAddress() );
    scion.SetSrcIA( src.GetIA() );
    scion.SetSrcAddress( src.GetHostAddress() );
  

    UdpHeader udp; // inner udp header

    udp.SetSourcePort( src.GetPort() );
    udp.SetDestinationPort( dst.GetPort() );

    p->AddHeader( udp );
    p->AddHeader( scion );

    return PConnWriteTo( p, flags, nextHop );

}

std::tuple< SCIONAddress, SNETPath>
SCIONUdpSocketImpl::readMsg(Ptr<Packet>& p, Address& fromAddr, uint32_t maxSize, uint32_t flags )
{
    Address lastHop;
   DecodedPacket dp =  m_scmp_inter.ReadFrom( p,lastHop, maxSize, flags );

    fromAddr = lastHop;
   SNETPath path;
   path.SetDataPlanePath( dp._scion.GetPath() );
   path.SetUnderlayNextHop( lastHop );

   SCIONAddress remote{ dp._scion.GetSrcIA(),dp._scion.SrcAddress() };
   remote.SetPort( dp._udp.GetSourcePort() );
   return { remote,path };
}

// corresponds to net.UDPConn.WriteTo
int SCIONUdpSocketImpl::PConnWriteTo( Ptr<Packet> p, uint32_t flags, const Address& address )
{
    if (InetSocketAddress::IsMatchingType(address))
    {
        InetSocketAddress transport = InetSocketAddress::ConvertFrom(address);
        Ipv4Address ipv4 = transport.GetIpv4();
        uint16_t port = transport.GetPort();
        uint8_t tos = transport.GetTos();
        return DoSendTo(p, ipv4, port, tos);
    }
    else if (Inet6SocketAddress::IsMatchingType(address))
    {
        Inet6SocketAddress transport = Inet6SocketAddress::ConvertFrom(address);
        Ipv6Address ipv6 = transport.GetIpv6();
        uint16_t port = transport.GetPort();
        return DoSendTo(p, ipv6, port);
    }
    return -1;
}
/*
Address SCIONUdpSocketImpl::FillSCIONHeaderFromAddr( SCIONHeader& scion, SCIONUDPAddress& add )
{   Address nextHop; // eine Inet oder Inet6 address

    // TODO create SCIONStack aggregate similar to Ipv4 Stack 
    // that knows the AS, ISD stuff etc

    scion.SetSrcIA( localIA );

    scion.SetSrcAddress( LocalListenAddress() ); // thisSocketsLocalListenAddress (ohne port)
    scion.SetDstIA( add._ia );
    scion.SetDstAddress( add._host aber ohne port, sondern nur Ipv4/6 addresse );

    scion.SetPath( add._path );

    nextHop = add._nextHop;

   
   // if nextHop == nil && LocalIA == add._ia)
  //  {
//			port := add.Host.Port
//			if add.Host.Port >= topology.HostPortRangeLow &&
//				add.Host.Port <= topology.HostPortRangeHigh 
//          {
//				port = topology.EndhostPort
//			}
//			nextHop = &net.UDPAddr{
//				IP:   add.Host.IP,
//				Port: port,
//				Zone: a.ddHost.Zone,
//			}
   



    return nextHop;
}*/

int
SCIONUdpSocketImpl::SendTo(Ptr<Packet> p, uint32_t flags, const Address& d)
{
    NS_LOG_FUNCTION(this << p << flags << d);


    SCIONAddress dst { SCIONAddress::ConvertFrom(d)};

/*
    // check if address is a scion Address 
    // add the outer UDP header
    // add the SCION Header

    SCIONHeader scion;
    Address nextHop;

    if( SCIONUDPAddress::IsMatchingType(address) )
    {
        SCIONUDPAddress add = SCIONUDPAddress::ConvertFrom( address );
        Address nextHop = FillSCIONHeaderFromAddr( scion, add );

    }else if ( false )// SCIONSVCAddress
    {

    }else
    {
        // cannot create a SCIONHeader from i.e. just Ip4/6 Addresses
        NS_ASSERT(false);
    }

    WriteTo( p, flags,nextHop );
    */
    /*
    func (c *scionConnWriter) WriteTo(b []byte, raddr net.Addr) (int, error) {
	var (
		dst     SCIONAddress
		port    int
		path    DataplanePath
		nextHop *net.UDPAddr
	)

	switch a := raddr.(type) {
	case nil:
		return 0, serrors.New("Missing remote address")
	case *UDPAddr:
		hostIP, ok := netip.AddrFromSlice(a.Host.IP)
		if !ok {
			return 0, serrors.New("invalid destination host IP", "ip", a.Host.IP)
		}
		dst = SCIONAddress{IA: a.IA,
                             Host: addr.HostIP(hostIP)
                             }
		port = a.Host.Port,
        path =  a.Path
		nextHop = a.NextHop

        // does this mean we are the last scion hop ?!
		if nextHop == nil && c.base.scionNet.LocalIA.Equal(a.IA) {
			port := a.Host.Port
			if a.Host.Port >= topology.HostPortRangeLow &&
				a.Host.Port <= topology.HostPortRangeHigh {
				port = topology.EndhostPort
			}
			nextHop = &net.UDPAddr{
				IP:   a.Host.IP,
				Port: port,
				Zone: a.Host.Zone,
			}

		}
	case *SVCAddr:
        port = 0
        path = a.Path
		dst SCIONAddress{IA: a.IA,
                        Host: addr.HostSVC(a.SVC)
                         }
                
                     
		nextHop = a.NextHop
	default:
		return 0, serrors.New("Unable to write to non-SCION address",
			"addr", fmt.Sprintf("%v(%T)", a, a))
	}

	listenHostIP, ok := netip.AddrFromSlice(c.base.listen.Host.IP)

    // listen was set in SCIONNetwork.Listen( ..)
    //	listen: &UDPAddr{
	//		IA:   n.LocalIA,
	//		Host: packetConn.LocalAddr().(*net.UDPAddr),
	//	},
    // 
	if !ok {
		return 0, serrors.New("invalid listen host IP", "ip", c.base.listen.Host.IP)
	}

	pkt := &Packet{
		Bytes: Bytes(c.buffer),
		PacketInfo: PacketInfo{
			Destination: dst,// SCIONHeader. DstIA = p.Destination.IA === dst.IA
							// SCIONHeader.SetDstAddr (p.Destination.Host )  == dst.Host ?== hostIp
			Source: SCIONAddress{
				IA:   c.base.scionNet.LocalIA,  // SCIONHeader.SrcIA = p.Source.IA (p is the packetinfo)
				Host: addr.HostIP(listenHostIP), // SCIONHeader.SetSrcAddr( p.Source.Host )
			},
			Path: path,
			Payload: UDPPayload{
				SrcPort: uint16(c.base.listen.Host.Port),
				DstPort: uint16(port),
				Payload: b,
			},
		},
	}

    // here the packet will be serialized and finally send
	if err := c.conn.WriteTo(pkt, nextHop); err != nil {     // m_scmp_inter->SendTo( pkt, nextHop )
		return 0, err
	}
	return len(b), nil
    */


// return PConnWriteTo(p,flags,address );

std::shared_ptr<SNETPath> path = nullptr;
if( GetNode()->GetObject<SCIONContext>()->LocalIA() != dst.GetIA() )
{
    path = std::make_shared<SNETPath>( m_replySelector.Path(dst) );
    NS_ASSERT( path != nullptr );
}

return SendToVia(p,flags,dst,path);

}

Ptr<Packet> SCIONUdpSocketImpl::RecvFromVia(uint32_t maxSize, uint32_t flags, Address& fromAddress )
{
    Ptr<Packet> packet = Create<Packet>();
    [[maybe_unused]] Address from; // lastUnderlayHop where the packet came from
    auto [remote, path ] = readMsg(packet,from, maxSize, flags );
    // read Msg should be shared_ptr here, as it can be null
    path.Reverse();

    m_replySelector.Record(path,remote);

    fromAddress = remote; // return the remote scion address
    return packet;

}
 int SCIONUdpSocketImpl::SendToVia(Ptr<Packet> p, uint32_t flags, const Address& dst, std::shared_ptr<SNETPath> pathToDst)
 {
    return writeMsg( m_local , SCIONAddress::ConvertFrom(dst), pathToDst,flags, p );
 }


// corresponds to func (c *scionConnReader) Read(b []byte) (int, error) {
Ptr<Packet>
SCIONUdpSocketImpl::Recv(uint32_t maxSize, uint32_t flags)
{
    NS_LOG_FUNCTION(this << maxSize << flags);

/*  // original 
     Address fromAddress;
    Ptr<Packet> packet = RecvFrom(maxSize, flags, fromAddress);
    return packet; */

    Ptr<Packet> packet = Create<Packet>();
    Address from; // lastUnderlayHop where the packet came from
    auto [remote, path ] = readMsg(packet,from, maxSize, flags );

    if( remote != SCIONAddress::ConvertFrom(m_defaultAddress))
    {
        // spurious packet from wrong source
        // reschedule this Recv Event ?!
    }

    return packet;
}


Ptr<Packet>
SCIONUdpSocketImpl::RecvFrom(uint32_t maxSize, uint32_t flags, Address& fromAddress)
{

Ptr<Packet> p = RecvFromVia( maxSize,  flags, fromAddress );

// old low low level
// return   DoRecvFromUDP(maxSize,flags,fromAddress);
return p;
}

/*
// corresponds to func (c *scionConnReader) ReadFrom(b []byte) (int, net.Addr, error) 
// where the address is actually an snet.UDPAddr
Ptr<Packet>
SCIONUdpSocketImpl::RecvFrom(uint32_t maxSize, uint32_t flags, Address& fromAddress) // maybe the fromAddress should be SCIONUdpAddress
{
    Ptr<Packet> p = Create<Packet>();
    SCIONUDPAddress returnAddr; // this is what will get asiigned to fromAddress in the end

    Address lastHop;  // an InetAddress or Inet6Address that should have a port set
    DecodedPacket dp = m_scmp_inter.ReadFrom(p,lastHop,maxSize,flags );


 //   SCIONHeader scion;
  //  p->RemoveHeader(scion);
//    UdpHeader udp; // inner udp header
//    p->RemoveHeader(udp);
//    auto srcport = udp.GetSourcePort();
//    

    auto reply_path = m_reply.ComputeReplyPath( dp._scion.GetPath() );


    returnAddr._ia = dp._scion.GetSrcIA();
    auto srcaddr =  dp._scion.SrcAddress(); // srcaddr is an Ip or Ipv6Address
    auto s = SetPort( srcaddr, dp._udp.GetSourcePort() ); // s is now a InetSocketAddr or Inet6
  //  srcaddr.SetPort( dp._udp.GetSourcePort() );
    returnAddr._host = srcaddr;

    returnAddr._nextHop = lastHop;
    returnAddr._path = reply_path;

    return p; // the only thing left in the packet now is its payload :)
   
//	pkt := Packet{
//		Bytes: Bytes(c.buffer),
//	}
//	var lastHop net.UDPAddr
//	err := c.conn.ReadFrom(&pkt, &lastHop)  // <<--- SCMPInterceptor::ReadFrom
//	if err != nil {
//		return 0, nil, err
//	}
//
//	rpath, ok := pkt.Path.(RawPath)
//	if !ok {
//		return 0, nil, serrors.New("unexpected path", "type", common.TypeOf(pkt.Path))
//	}
//	replyPath, err := c.replyPather.ReplyPath(rpath)
//	if err != nil {
//		return 0, nil, serrors.WrapStr("creating reply path", err)
//	}
//
//	udp, ok := pkt.Payload.(UDPPayload)
//	if !ok {
//		return 0, nil, serrors.New("unexpected payload", "type", common.TypeOf(pkt.Payload))
//	}
//	n := copy(b, udp.Payload)
//
//		IA: pkt.Source.IA,
//		Host: &net.UDPAddr{
//            // in packet.Decode() the Source field was set as follows:
//            // 	p.Source = SCIONAddress{IA: scionLayer.SrcIA, Host: srcAddr(=scionLayer.srcAddress) }
//			IP:   pkt.Source.Host.IP().AsSlice(), // a 4, or 16 byte array containing the ipv4/6 address
//            //  PacketInfo::Source SCIONAddress (=addr.Addr) IP()->returns netaddr.Ip
//			Port: int(udp.SrcPort),
//		},
//		Path:    replyPath,
//		NextHop: CopyUDPAddr(&lastHop),
//	}
//	return n, remote, nil


// old low low level
// return   DoRecvFromUDP(maxSize,flags,fromAddress);
}
*/

/*
this is as low as it gets in layers of abstraction.
Corresponds to net.UDPCon.ReadFrom in go.
Contains 1:1 copy of ns3::UDPSocketImpl code
*/
Ptr<Packet>
SCIONUdpSocketImpl::DoRecvFromUDP( uint32_t maxSize, uint32_t flags, Address& fromAddress )
{
     NS_LOG_FUNCTION(this << maxSize << flags);

    if (m_deliveryQueue.empty())
    {
        m_errno = ERROR_AGAIN;
        return nullptr;
    }
    Ptr<Packet> p = m_deliveryQueue.front().first;
    fromAddress = m_deliveryQueue.front().second;

    if (p->GetSize() <= maxSize)
    {
        m_deliveryQueue.pop();
        m_rxAvailable -= p->GetSize();
    }
    else
    {
        p = nullptr;
    }
    return p;
}

int
SCIONUdpSocketImpl::GetSockName(Address& address) const
{
    NS_LOG_FUNCTION(this << address);
    if (m_endPoint != nullptr)
    {
        address = InetSocketAddress(m_endPoint->GetLocalAddress(), m_endPoint->GetLocalPort());
    }
    else if (m_endPoint6 != nullptr)
    {
        address = Inet6SocketAddress(m_endPoint6->GetLocalAddress(), m_endPoint6->GetLocalPort());
    }
    else
    { // It is possible to call this method on a socket without a name
        // in which case, behavior is unspecified
        // Should this return an InetSocketAddress or an Inet6SocketAddress?
        address = InetSocketAddress(Ipv4Address::GetZero(), 0);
    }
    return 0;
}

int
SCIONUdpSocketImpl::GetPeerName(Address& address) const
{
    NS_LOG_FUNCTION(this << address);

    if (!m_connected)
    {
        m_errno = ERROR_NOTCONN;
        return -1;
    }

    if (Ipv4Address::IsMatchingType(m_defaultAddress))
    {
        Ipv4Address addr = Ipv4Address::ConvertFrom(m_defaultAddress);
        InetSocketAddress inet(addr, m_defaultPort);
        inet.SetTos(GetIpTos());
        address = inet;
    }
    else if (Ipv6Address::IsMatchingType(m_defaultAddress))
    {
        Ipv6Address addr = Ipv6Address::ConvertFrom(m_defaultAddress);
        address = Inet6SocketAddress(addr, m_defaultPort);
    }
    else
    {
        NS_ASSERT_MSG(false, "unexpected address type");
    }

    return 0;
}

/*
void SCIONUdpSocketImpl::WriteTo( Ptr<Packet> pkt, uint32_t flags, Address& addr )
{
   // DoSendToUDP(pkt,flags,addr);
    
    if err := pkt.Serialize(); err != nil {
		return serrors.WrapStr("serialize SCION packet", err)
	}

	// Send message
	n, err := c.Conn.WriteTo(pkt.Bytes, ov)      // net.UDPConn::WriteTo
	if err != nil {
		return serrors.WrapStr("Reliable socket write error", err)
	}
	metrics.CounterAdd(c.Metrics.WriteBytes, float64(n))
	metrics.CounterInc(c.Metrics.WritePackets)
	return nil
    
}
*/


SCIONUdpSocketImpl::SCMPInterceptor::SCMPInterceptor(SCIONUdpSocketImpl* sock )
: m_socket(sock)
{}

SCIONUdpSocketImpl::DecodedPacket SCIONUdpSocketImpl::SCMPInterceptor::ReadFrom( Ptr<Packet>& pkt, Address& addr, uint32_t maxSize, uint32_t flags )
{
   auto[ dp,isScmp] =  DoReadFrom( pkt,addr, maxSize, flags );

    /*
    check if the packet had SCMP payload,
    if so, pass it to the SCMPHandler if we have one installed

    */
   if( m_scmp && isScmp )
   {
    // all headers can be removed from the packet, if passed in here
    // handler looks only at payload
    m_scmp->HandleSCMP( pkt );
   }
   return dp;
    /*
    	for {
		// Read until we get an error or a data packet
        // the only error that could get reported here 
        // are network errors, which cant happen in a simulation ( at least i think so )
        // after this call, the packet pkt is completely decoded
		if err := c.readFrom(pkt, ov); err != nil { // <----- SCMPInterceptor::DoReadFrom
			return err
		}
		if scmp, ok := pkt.Payload.(SCMPPayload); ok {
			if c.SCMPHandler == nil {
				metrics.CounterInc(c.Metrics.SCMPErrors)
				return serrors.New("scmp packet received, but no handler found",
					"type_code", slayers.CreateSCMPTypeCode(scmp.Type(), scmp.Code()),
					"src", pkt.Source)
			}
			if err := c.SCMPHandler.Handle(pkt); err != nil {
				// Return error intact s.t. applications can handle custom
				// error types returned by SCMP handlers.
				return err
			}
			continue
		}
		// non-SCMP L4s are assumed to be data and get passed back to the
		// app.
		return nil
        */
	
}


std::pair< SCIONUdpSocketImpl::DecodedPacket,bool> 
SCIONUdpSocketImpl::SCMPInterceptor::DoReadFrom( Ptr<Packet>& pkt, Address& addr,uint32_t maxSize, uint32_t flags )
{
    // low level read from underlying udp/ip socket 
   pkt = m_socket->DoRecvFromUDP( maxSize,flags, addr );

   DecodedPacket dp;
    bool isScmpPayload = false;
    pkt->RemoveHeader(dp._scion);

    if( dp._scion.NextHeader() == L4ProtocolType_t::L4SCMP )
    {
        isScmpPayload = true;
        /* parse the SCMP Payload
           and store it in the DecodedPacket
           Or dont, leave it for SCMPHandler::Handle(Ptr<Packet> ) in the next step
        */

    } 
    else if ( dp._scion.NextHeader() == L4ProtocolType_t::L4UDP )
    {   // this is the normal happy code path, we want to see :)
        pkt->RemoveHeader(dp._udp); // inner udp header
    }

   return {dp,isScmpPayload};

 /*   pkt.Prepare()
	n, lastHopNetAddr, err := c.Conn.ReadFrom(pkt.Bytes) // <--- m_socket-> DoRecvFrom
	if err != nil {
		metrics.CounterInc(c.Metrics.UnderlayConnectionErrors)
		return serrors.WrapStr("Reliable socket read error", err)
	}
	metrics.CounterAdd(c.Metrics.ReadBytes, float64(n))
	metrics.CounterInc(c.Metrics.ReadPackets)

	pkt.Bytes = pkt.Bytes[:n]
	var lastHop *net.UDPAddr

	var ok bool
	lastHop, ok = lastHopNetAddr.(*net.UDPAddr)
	if !ok {
		return serrors.New("Invalid lastHop address Type",
			"Actual", lastHopNetAddr)
	}

	if err := pkt.Decode(); err != nil {
		metrics.CounterInc(c.Metrics.ParseErrors)
		return serrors.WrapStr("decoding packet", err)
	}

	if ov != nil {
		*ov = *lastHop
	}
	return nil
*/

}
} // namespace ns3

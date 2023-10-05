#include "ns3/scion-udp-socket-impl.h"

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


#include "ns3/udp-header.h"
#include "ns3/scion-header.h"
#include "ns3/scion-address.h"

#include <limits>


/*
this file contains implementations of all methods of SCIONUdpSocketImpl 
which are mere copies of UdpSocketImpl
*/
namespace ns3
{
 NS_LOG_COMPONENT_DEFINE("SCIONUdpSocketImpl2");
    // The correct maximum UDP message size is 65507, as determined by the following formula:
// 0xffff - (sizeof(IP Header) + sizeof(UDP Header)) = 65535-(20+8) = 65507
// \todo MAX_IPV4_UDP_DATAGRAM_SIZE is correct only for IPv4
static const inline uint32_t MAX_IPV4_UDP_DATAGRAM_SIZE = 65507; //!< Maximum UDP datagram size

int
SCIONUdpSocketImpl::Listen()
{
    m_errno = Socket::ERROR_OPNOTSUPP;
    return -1;
}

uint32_t
SCIONUdpSocketImpl::GetRxAvailable() const
{
    NS_LOG_FUNCTION(this);
    // We separately maintain this state to avoid walking the queue
    // every time this might be called
    return m_rxAvailable;
}


int
SCIONUdpSocketImpl::MulticastJoinGroup(uint32_t interface, const Address& groupAddress)
{
    NS_LOG_FUNCTION(interface << groupAddress);
    /*
     1) sanity check interface
     2) sanity check that it has not been called yet on this interface/group
     3) determine address family of groupAddress
     4) locally store a list of (interface, groupAddress)
     5) call ipv4->MulticastJoinGroup () or Ipv6->MulticastJoinGroup ()
    */
    return 0;
}

int
SCIONUdpSocketImpl::MulticastLeaveGroup(uint32_t interface, const Address& groupAddress)
{
    NS_LOG_FUNCTION(interface << groupAddress);
    /*
     1) sanity check interface
     2) determine address family of groupAddress
     3) delete from local list of (interface, groupAddress); raise a LOG_WARN
        if not already present (but return 0)
     5) call ipv4->MulticastLeaveGroup () or Ipv6->MulticastLeaveGroup ()
    */
    return 0;
}

void
SCIONUdpSocketImpl::BindToNetDevice(Ptr<NetDevice> netdevice)
{
    NS_LOG_FUNCTION(netdevice);

    Ptr<NetDevice> oldBoundNetDevice = m_boundnetdevice;

    Socket::BindToNetDevice(netdevice); // Includes sanity check
    if (m_endPoint != nullptr)
    {
        m_endPoint->BindToNetDevice(netdevice);
    }

    if (m_endPoint6 != nullptr)
    {
        m_endPoint6->BindToNetDevice(netdevice);

        // The following is to fix the multicast distribution inside the node
        // and to upgrade it to the actual bound NetDevice.
        if (m_endPoint6->GetLocalAddress().IsMulticast())
        {
            Ptr<Ipv6L3Protocol> ipv6l3 = m_node->GetObject<Ipv6L3Protocol>();
            if (ipv6l3)
            {
                // Cleanup old one
                if (oldBoundNetDevice)
                {
                    uint32_t index = ipv6l3->GetInterfaceForDevice(oldBoundNetDevice);
                    ipv6l3->RemoveMulticastAddress(m_endPoint6->GetLocalAddress(), index);
                }
                else
                {
                    ipv6l3->RemoveMulticastAddress(m_endPoint6->GetLocalAddress());
                }
                // add new one
                if (netdevice)
                {
                    uint32_t index = ipv6l3->GetInterfaceForDevice(netdevice);
                    ipv6l3->AddMulticastAddress(m_endPoint6->GetLocalAddress(), index);
                }
                else
                {
                    ipv6l3->AddMulticastAddress(m_endPoint6->GetLocalAddress());
                }
            }
        }
    }
}

void
SCIONUdpSocketImpl::ForwardUp(Ptr<Packet> packet,
                         Ipv4Header header,
                         uint16_t port,
                         Ptr<Ipv4Interface> incomingInterface)
{
    NS_LOG_FUNCTION(this << packet << header << port);

    if (m_shutdownRecv)
    {
        return;
    }

    // Should check via getsockopt ()..
    if (IsRecvPktInfo())
    {
        Ipv4PacketInfoTag tag;
        packet->RemovePacketTag(tag);
        tag.SetAddress(header.GetDestination());
        tag.SetTtl(header.GetTtl());
        tag.SetRecvIf(incomingInterface->GetDevice()->GetIfIndex());
        packet->AddPacketTag(tag);
    }

    // Check only version 4 options
    if (IsIpRecvTos())
    {
        SocketIpTosTag ipTosTag;
        ipTosTag.SetTos(header.GetTos());
        packet->AddPacketTag(ipTosTag);
    }

    if (IsIpRecvTtl())
    {
        SocketIpTtlTag ipTtlTag;
        ipTtlTag.SetTtl(header.GetTtl());
        packet->AddPacketTag(ipTtlTag);
    }

    // in case the packet still has a priority tag attached, remove it
    SocketPriorityTag priorityTag;
    packet->RemovePacketTag(priorityTag);

    if ((m_rxAvailable + packet->GetSize()) <= m_rcvBufSize)
    {
        Address address = InetSocketAddress(header.GetSource(), port);
        m_deliveryQueue.emplace(packet, address);
        m_rxAvailable += packet->GetSize();
        NotifyDataRecv();
    }
    else
    {
        // In general, this case should not occur unless the
        // receiving application reads data from this socket slowly
        // in comparison to the arrival rate
        //
        // drop and trace packet
        NS_LOG_WARN("No receive buffer space available.  Drop.");
        m_dropTrace(packet);
    }
}

void
SCIONUdpSocketImpl::ForwardUp6(Ptr<Packet> packet,
                          Ipv6Header header,
                          uint16_t port,
                          Ptr<Ipv6Interface> incomingInterface)
{
    NS_LOG_FUNCTION(this << packet << header.GetSource() << port);

    if (m_shutdownRecv)
    {
        return;
    }

    // Should check via getsockopt ().
    if (IsRecvPktInfo())
    {
        Ipv6PacketInfoTag tag;
        packet->RemovePacketTag(tag);
        tag.SetAddress(header.GetDestination());
        tag.SetHoplimit(header.GetHopLimit());
        tag.SetTrafficClass(header.GetTrafficClass());
        tag.SetRecvIf(incomingInterface->GetDevice()->GetIfIndex());
        packet->AddPacketTag(tag);
    }

    // Check only version 6 options
    if (IsIpv6RecvTclass())
    {
        SocketIpv6TclassTag ipTclassTag;
        ipTclassTag.SetTclass(header.GetTrafficClass());
        packet->AddPacketTag(ipTclassTag);
    }

    if (IsIpv6RecvHopLimit())
    {
        SocketIpv6HopLimitTag ipHopLimitTag;
        ipHopLimitTag.SetHopLimit(header.GetHopLimit());
        packet->AddPacketTag(ipHopLimitTag);
    }

    // in case the packet still has a priority tag attached, remove it
    SocketPriorityTag priorityTag;
    packet->RemovePacketTag(priorityTag);

    if ((m_rxAvailable + packet->GetSize()) <= m_rcvBufSize)
    {
        Address address = Inet6SocketAddress(header.GetSource(), port);
        m_deliveryQueue.emplace(packet, address);
        m_rxAvailable += packet->GetSize();
        NotifyDataRecv();
    }
    else
    {
        // In general, this case should not occur unless the
        // receiving application reads data from this socket slowly
        // in comparison to the arrival rate
        //
        // drop and trace packet
        NS_LOG_WARN("No receive buffer space available.  Drop.");
        m_dropTrace(packet);
    }
}

void
SCIONUdpSocketImpl::ForwardIcmp(Ipv4Address icmpSource,
                           uint8_t icmpTtl,
                           uint8_t icmpType,
                           uint8_t icmpCode,
                           uint32_t icmpInfo)
{
    NS_LOG_FUNCTION(this << icmpSource << (uint32_t)icmpTtl << (uint32_t)icmpType << (uint32_t)icmpCode << icmpInfo);
    if (!m_icmpCallback.IsNull())
    {
        m_icmpCallback(icmpSource, icmpTtl, icmpType, icmpCode, icmpInfo);
    }
}

void
SCIONUdpSocketImpl::ForwardIcmp6(Ipv6Address icmpSource,
                            uint8_t icmpTtl,
                            uint8_t icmpType,
                            uint8_t icmpCode,
                            uint32_t icmpInfo)
{
    NS_LOG_FUNCTION(this << icmpSource << (uint32_t)icmpTtl << (uint32_t)icmpType << (uint32_t)icmpCode << icmpInfo);
    if (!m_icmpCallback6.IsNull())
    {
        m_icmpCallback6(icmpSource, icmpTtl, icmpType, icmpCode, icmpInfo);
    }
}

void
SCIONUdpSocketImpl::SetRcvBufSize(uint32_t size)
{
    m_rcvBufSize = size;
}

uint32_t
SCIONUdpSocketImpl::GetRcvBufSize() const
{
    return m_rcvBufSize;
}

void
SCIONUdpSocketImpl::SetIpMulticastTtl(uint8_t ipTtl)
{
    m_ipMulticastTtl = ipTtl;
}

uint8_t
SCIONUdpSocketImpl::GetIpMulticastTtl() const
{
    return m_ipMulticastTtl;
}

void
SCIONUdpSocketImpl::SetIpMulticastIf(int32_t ipIf)
{
    m_ipMulticastIf = ipIf;
}

int32_t
SCIONUdpSocketImpl::GetIpMulticastIf() const
{
    return m_ipMulticastIf;
}

void
SCIONUdpSocketImpl::SetIpMulticastLoop(bool loop)
{
    m_ipMulticastLoop = loop;
}

bool
SCIONUdpSocketImpl::GetIpMulticastLoop() const
{
    return m_ipMulticastLoop;
}

void
SCIONUdpSocketImpl::SetMtuDiscover(bool discover)
{
    m_mtuDiscover = discover;
}

bool
SCIONUdpSocketImpl::GetMtuDiscover() const
{
    return m_mtuDiscover;
}

bool
SCIONUdpSocketImpl::SetAllowBroadcast(bool allowBroadcast)
{
    m_allowBroadcast = allowBroadcast;
    return true;
}

bool
SCIONUdpSocketImpl::GetAllowBroadcast() const
{
    return m_allowBroadcast;
}

void
SCIONUdpSocketImpl::Ipv6JoinGroup(Ipv6Address address,
                             Socket::Ipv6MulticastFilterMode filterMode,
                             std::vector<Ipv6Address> sourceAddresses)
{
    NS_LOG_FUNCTION(this << address << &filterMode << &sourceAddresses);

    // We can join only one multicast group (or change its params)
    NS_ASSERT_MSG((m_ipv6MulticastGroupAddress == address || m_ipv6MulticastGroupAddress.IsAny()),
                  "Can join only one IPv6 multicast group.");

    m_ipv6MulticastGroupAddress = address;

    Ptr<Ipv6L3Protocol> ipv6l3 = m_node->GetObject<Ipv6L3Protocol>();
    if (ipv6l3)
    {
        if (filterMode == INCLUDE && sourceAddresses.empty())
        {
            // it is a leave
            if (m_boundnetdevice)
            {
                int32_t index = ipv6l3->GetInterfaceForDevice(m_boundnetdevice);
                NS_ASSERT_MSG(index >= 0, "Interface without a valid index");
                ipv6l3->RemoveMulticastAddress(address, index);
            }
            else
            {
                ipv6l3->RemoveMulticastAddress(address);
            }
        }
        else
        {
            // it is a join or a modification
            if (m_boundnetdevice)
            {
                int32_t index = ipv6l3->GetInterfaceForDevice(m_boundnetdevice);
                NS_ASSERT_MSG(index >= 0, "Interface without a valid index");
                ipv6l3->AddMulticastAddress(address, index);
            }
            else
            {
                ipv6l3->AddMulticastAddress(address);
            }
        }
    }
}



void
SCIONUdpSocketImpl::SetNode(Ptr<Node> node)
{
    NS_LOG_FUNCTION(this << node);
    m_node = node;
}

void
SCIONUdpSocketImpl::SetUdp(Ptr<UdpL4Protocol> udp)
{
    NS_LOG_FUNCTION(this << udp);
    m_udp = udp;
}

Socket::SocketErrno
SCIONUdpSocketImpl::GetErrno() const
{
    NS_LOG_FUNCTION(this); // requires g_log
    return m_errno;
}

Socket::SocketType
SCIONUdpSocketImpl::GetSocketType() const
{
    return NS3_SOCK_DGRAM;
}

Ptr<Node>
SCIONUdpSocketImpl::GetNode() const
{
    NS_LOG_FUNCTION(this); // requires g_log
    return m_node;
}

void
SCIONUdpSocketImpl::Destroy()
{
    NS_LOG_FUNCTION(this); // requires g_log
    if (m_udp)
    {
        m_udp->RemoveSocket(this);
    }
    m_endPoint = nullptr;
}

void
SCIONUdpSocketImpl::Destroy6()
{
    NS_LOG_FUNCTION(this); // requires g_log
    if (m_udp)
    {
        m_udp->RemoveSocket(this);
    }
    m_endPoint6 = nullptr;
}

/* Deallocate the end point and cancel all the timers */
void
SCIONUdpSocketImpl::DeallocateEndPoint()
{
    if (m_endPoint != nullptr)
    {
        m_udp->DeAllocate(m_endPoint);
        m_endPoint = nullptr;
    }
    if (m_endPoint6 != nullptr)
    {
        m_udp->DeAllocate(m_endPoint6);
        m_endPoint6 = nullptr;
    }
}


int
SCIONUdpSocketImpl::Bind()
{
    NS_LOG_FUNCTION(this); // requires g_log
    m_endPoint = m_udp->Allocate();
    if (m_boundnetdevice)
    {
        m_endPoint->BindToNetDevice(m_boundnetdevice);
    }
    return FinishBind();
}

int
SCIONUdpSocketImpl::Bind6()
{
    NS_LOG_FUNCTION(this); // requires g_log
    m_endPoint6 = m_udp->Allocate6();
    if (m_boundnetdevice)
    {
        m_endPoint6->BindToNetDevice(m_boundnetdevice);
    }
    return FinishBind();
}     

int
SCIONUdpSocketImpl::Bind(const Address& address)
{
    NS_LOG_FUNCTION(this << address);

    if (InetSocketAddress::IsMatchingType(address))
    {
        NS_ASSERT_MSG(m_endPoint == nullptr, "Endpoint already allocated.");

        InetSocketAddress transport = InetSocketAddress::ConvertFrom(address);
        Ipv4Address ipv4 = transport.GetIpv4();
        uint16_t port = transport.GetPort();
        SetIpTos(transport.GetTos());
        if (ipv4 == Ipv4Address::GetAny() && port == 0)
        {
            m_endPoint = m_udp->Allocate();
        }
        else if (ipv4 == Ipv4Address::GetAny() && port != 0)
        {
            m_endPoint = m_udp->Allocate(GetBoundNetDevice(), port);
        }
        else if (ipv4 != Ipv4Address::GetAny() && port == 0)
        {
            m_endPoint = m_udp->Allocate(ipv4);
        }
        else if (ipv4 != Ipv4Address::GetAny() && port != 0)
        {
            m_endPoint = m_udp->Allocate(GetBoundNetDevice(), ipv4, port);
        }
        if (nullptr == m_endPoint)
        {
            m_errno = port ? ERROR_ADDRINUSE : ERROR_ADDRNOTAVAIL;
            return -1;
        }
        if (m_boundnetdevice)
        {
            m_endPoint->BindToNetDevice(m_boundnetdevice);
        }
    }
    else if (Inet6SocketAddress::IsMatchingType(address))
    {
        NS_ASSERT_MSG(m_endPoint == nullptr, "Endpoint already allocated.");

        Inet6SocketAddress transport = Inet6SocketAddress::ConvertFrom(address);
        Ipv6Address ipv6 = transport.GetIpv6();
        uint16_t port = transport.GetPort();
        if (ipv6 == Ipv6Address::GetAny() && port == 0)
        {
            m_endPoint6 = m_udp->Allocate6();
        }
        else if (ipv6 == Ipv6Address::GetAny() && port != 0)
        {
            m_endPoint6 = m_udp->Allocate6(GetBoundNetDevice(), port);
        }
        else if (ipv6 != Ipv6Address::GetAny() && port == 0)
        {
            m_endPoint6 = m_udp->Allocate6(ipv6);
        }
        else if (ipv6 != Ipv6Address::GetAny() && port != 0)
        {
            m_endPoint6 = m_udp->Allocate6(GetBoundNetDevice(), ipv6, port);
        }
        if (nullptr == m_endPoint6)
        {
            m_errno = port ? ERROR_ADDRINUSE : ERROR_ADDRNOTAVAIL;
            return -1;
        }
        if (m_boundnetdevice)
        {
            m_endPoint6->BindToNetDevice(m_boundnetdevice);
        }

        if (ipv6.IsMulticast())
        {
            Ptr<Ipv6L3Protocol> ipv6l3 = m_node->GetObject<Ipv6L3Protocol>();
            if (ipv6l3)
            {
                if (!m_boundnetdevice)
                {
                    ipv6l3->AddMulticastAddress(ipv6);
                }
                else
                {
                    uint32_t index = ipv6l3->GetInterfaceForDevice(m_boundnetdevice);
                    ipv6l3->AddMulticastAddress(m_endPoint6->GetLocalAddress(), index);
                }
            }
        }
    }
    else
    {
        NS_LOG_ERROR("Not IsMatchingType");
        m_errno = ERROR_INVAL;
        return -1;
    }

    return FinishBind();
}

int
SCIONUdpSocketImpl::ShutdownSend()
{
    NS_LOG_FUNCTION(this); // requires g_log
    m_shutdownSend = true;
    return 0;
}



/* contains 1:1 the code of UDPSocketImpl */
int
SCIONUdpSocketImpl::DoSendToUDP(Ptr<Packet> p, Ipv4Address dest, uint16_t port, uint8_t tos)
{
    NS_LOG_FUNCTION(this << p << dest << port << (uint16_t)tos);
    if (m_boundnetdevice)
    {
        NS_LOG_LOGIC("Bound interface number " << m_boundnetdevice->GetIfIndex());
    }
    if (m_endPoint == nullptr)
    {
        if (Bind() == -1)
        {
            NS_ASSERT(m_endPoint == nullptr);
            return -1;
        }
        NS_ASSERT(m_endPoint != nullptr);
    }
    if (m_shutdownSend)
    {
        m_errno = ERROR_SHUTDOWN;
        return -1;
    }

    if (p->GetSize() > GetTxAvailable())
    {
        m_errno = ERROR_MSGSIZE;
        return -1;
    }

    uint8_t priority = GetPriority();
    if (tos)
    {
        SocketIpTosTag ipTosTag;
        ipTosTag.SetTos(tos);
        // This packet may already have a SocketIpTosTag (see BUG 2440)
        p->ReplacePacketTag(ipTosTag);
        priority = IpTos2Priority(tos);
    }

    if (priority)
    {
        SocketPriorityTag priorityTag;
        priorityTag.SetPriority(priority);
        p->ReplacePacketTag(priorityTag);
    }

    Ptr<Ipv4> ipv4 = m_node->GetObject<Ipv4>();

    // Locally override the IP TTL for this socket
    // We cannot directly modify the TTL at this stage, so we set a Packet tag
    // The destination can be either multicast, unicast/anycast, or
    // either all-hosts broadcast or limited (subnet-directed) broadcast.
    // For the latter two broadcast types, the TTL will later be set to one
    // irrespective of what is set in these socket options.  So, this tagging
    // may end up setting the TTL of a limited broadcast packet to be
    // the same as a unicast, but it will be fixed further down the stack
    if (m_ipMulticastTtl != 0 && dest.IsMulticast())
    {
        SocketIpTtlTag tag;
        tag.SetTtl(m_ipMulticastTtl);
        p->AddPacketTag(tag);
    }
    else if (IsManualIpTtl() && GetIpTtl() != 0 && !dest.IsMulticast() && !dest.IsBroadcast())
    {
        SocketIpTtlTag tag;
        tag.SetTtl(GetIpTtl());
        p->AddPacketTag(tag);
    }
    {
        SocketSetDontFragmentTag tag;
        bool found = p->RemovePacketTag(tag);
        if (!found)
        {
            if (m_mtuDiscover)
            {
                tag.Enable();
            }
            else
            {
                tag.Disable();
            }
            p->AddPacketTag(tag);
        }
    }

    // Note that some systems will only send limited broadcast packets
    // out of the "default" interface; here we send it out all interfaces
    if (dest.IsBroadcast())
    {
        if (!m_allowBroadcast)
        {
            m_errno = ERROR_OPNOTSUPP;
            return -1;
        }
        NS_LOG_LOGIC("Limited broadcast start.");
        for (uint32_t i = 0; i < ipv4->GetNInterfaces(); i++)
        {
            // Get the primary address
            Ipv4InterfaceAddress iaddr = ipv4->GetAddress(i, 0);
            Ipv4Address addri = iaddr.GetLocal();
            if (addri == Ipv4Address("127.0.0.1"))
            {
                continue;
            }
            // Check if interface-bound socket
            if (m_boundnetdevice)
            {
                if (ipv4->GetNetDevice(i) != m_boundnetdevice)
                {
                    continue;
                }
            }
            NS_LOG_LOGIC("Sending one copy from " << addri << " to " << dest);
            m_udp->Send(p->Copy(), addri, dest, m_endPoint->GetLocalPort(), port);
            NotifyDataSent(p->GetSize());
            NotifySend(GetTxAvailable());
        }
        NS_LOG_LOGIC("Limited broadcast end.");
        return p->GetSize();
    }
    else if (m_endPoint->GetLocalAddress() != Ipv4Address::GetAny())
    {
        m_udp->Send(p->Copy(),
                    m_endPoint->GetLocalAddress(),
                    dest,
                    m_endPoint->GetLocalPort(),
                    port,
                    nullptr);
        NotifyDataSent(p->GetSize());
        NotifySend(GetTxAvailable());
        return p->GetSize();
    }
    else if (ipv4->GetRoutingProtocol())
    {
        Ipv4Header header;
        header.SetDestination(dest);
        header.SetProtocol(UdpL4Protocol::PROT_NUMBER);
        Socket::SocketErrno errno_;
        Ptr<Ipv4Route> route;
        Ptr<NetDevice> oif = m_boundnetdevice; // specify non-zero if bound to a specific device
        // TBD-- we could cache the route and just check its validity
        route = ipv4->GetRoutingProtocol()->RouteOutput(p, header, oif, errno_);
        if (route)
        {
            NS_LOG_LOGIC("Route exists");
            if (!m_allowBroadcast)
            {
                // Here we try to route subnet-directed broadcasts
                uint32_t outputIfIndex = ipv4->GetInterfaceForDevice(route->GetOutputDevice());
                uint32_t ifNAddr = ipv4->GetNAddresses(outputIfIndex);
                for (uint32_t addrI = 0; addrI < ifNAddr; ++addrI)
                {
                    Ipv4InterfaceAddress ifAddr = ipv4->GetAddress(outputIfIndex, addrI);
                    if (dest == ifAddr.GetBroadcast())
                    {
                        m_errno = ERROR_OPNOTSUPP;
                        return -1;
                    }
                }
            }

            header.SetSource(route->GetSource());
            m_udp->Send(p->Copy(),
                        header.GetSource(),
                        header.GetDestination(),
                        m_endPoint->GetLocalPort(),
                        port,
                        route);
            NotifyDataSent(p->GetSize());
            return p->GetSize();
        }
        else
        {
            NS_LOG_LOGIC("No route to destination");
            NS_LOG_ERROR(errno_);
            m_errno = errno_;
            return -1;
        }
    }
    else
    {
        NS_LOG_ERROR("ERROR_NOROUTETOHOST");
        m_errno = ERROR_NOROUTETOHOST;
        return -1;
    }

    return 0;
}


/*
contains 1:1 the code of UDPSocketImpl
*/
int SCIONUdpSocketImpl::DoSendUDP( Ptr<Packet> p)
{
       NS_LOG_FUNCTION(this << p);
    if (m_endPoint == nullptr && Ipv4Address::IsMatchingType(m_defaultAddress))
    {
        if (Bind() == -1)
        {
            NS_ASSERT(m_endPoint == nullptr);
            return -1;
        }
        NS_ASSERT(m_endPoint != nullptr);
    }
    else if (m_endPoint6 == nullptr && Ipv6Address::IsMatchingType(m_defaultAddress))
    {
        if (Bind6() == -1)
        {
            NS_ASSERT(m_endPoint6 == nullptr);
            return -1;
        }
        NS_ASSERT(m_endPoint6 != nullptr);
    }
    if (m_shutdownSend)
    {
        m_errno = ERROR_SHUTDOWN;
        return -1;
    }

    if (Ipv4Address::IsMatchingType(m_defaultAddress))
    {
        return DoSendTo(p, Ipv4Address::ConvertFrom(m_defaultAddress), m_defaultPort, GetIpTos());
    }
    else if (Ipv6Address::IsMatchingType(m_defaultAddress))
    {
        return DoSendTo(p, Ipv6Address::ConvertFrom(m_defaultAddress), m_defaultPort);
    }

    m_errno = ERROR_AFNOSUPPORT;
    return (-1);
}


int
SCIONUdpSocketImpl::DoSendToUdp(Ptr<Packet> p, Ipv6Address dest, uint16_t port)
{
    NS_LOG_FUNCTION(this << p << dest << port);

    if (dest.IsIpv4MappedAddress())
    {
        return (DoSendTo(p, dest.GetIpv4MappedAddress(), port, 0));
    }
    if (m_boundnetdevice)
    {
        NS_LOG_LOGIC("Bound interface number " << m_boundnetdevice->GetIfIndex());
    }
    if (m_endPoint6 == nullptr)
    {
        if (Bind6() == -1)
        {
            NS_ASSERT(m_endPoint6 == nullptr);
            return -1;
        }
        NS_ASSERT(m_endPoint6 != nullptr);
    }
    if (m_shutdownSend)
    {
        m_errno = ERROR_SHUTDOWN;
        return -1;
    }

    if (p->GetSize() > GetTxAvailable())
    {
        m_errno = ERROR_MSGSIZE;
        return -1;
    }

    if (IsManualIpv6Tclass())
    {
        SocketIpv6TclassTag ipTclassTag;
        ipTclassTag.SetTclass(GetIpv6Tclass());
        p->AddPacketTag(ipTclassTag);
    }

    uint8_t priority = GetPriority();
    if (priority)
    {
        SocketPriorityTag priorityTag;
        priorityTag.SetPriority(priority);
        p->ReplacePacketTag(priorityTag);
    }

    Ptr<Ipv6> ipv6 = m_node->GetObject<Ipv6>();

    // Locally override the IP TTL for this socket
    // We cannot directly modify the TTL at this stage, so we set a Packet tag
    // The destination can be either multicast, unicast/anycast, or
    // either all-hosts broadcast or limited (subnet-directed) broadcast.
    // For the latter two broadcast types, the TTL will later be set to one
    // irrespective of what is set in these socket options.  So, this tagging
    // may end up setting the TTL of a limited broadcast packet to be
    // the same as a unicast, but it will be fixed further down the stack
    if (m_ipMulticastTtl != 0 && dest.IsMulticast())
    {
        SocketIpv6HopLimitTag tag;
        tag.SetHopLimit(m_ipMulticastTtl);
        p->AddPacketTag(tag);
    }
    else if (IsManualIpv6HopLimit() && GetIpv6HopLimit() != 0 && !dest.IsMulticast())
    {
        SocketIpv6HopLimitTag tag;
        tag.SetHopLimit(GetIpv6HopLimit());
        p->AddPacketTag(tag);
    }
    // There is no analogous to an IPv4 broadcast address in IPv6.
    // Instead, we use a set of link-local, site-local, and global
    // multicast addresses.  The Ipv6 routing layers should all
    // provide an interface-specific route to these addresses such
    // that we can treat these multicast addresses as "not broadcast"

    if (m_endPoint6->GetLocalAddress() != Ipv6Address::GetAny())
    {
        m_udp->Send(p->Copy(),
                    m_endPoint6->GetLocalAddress(),
                    dest,
                    m_endPoint6->GetLocalPort(),
                    port,
                    nullptr);
        NotifyDataSent(p->GetSize());
        NotifySend(GetTxAvailable());
        return p->GetSize();
    }
    else if (ipv6->GetRoutingProtocol())
    {
        Ipv6Header header;
        header.SetDestination(dest);
        header.SetNextHeader(UdpL4Protocol::PROT_NUMBER);
        Socket::SocketErrno errno_;
        Ptr<Ipv6Route> route;
        Ptr<NetDevice> oif = m_boundnetdevice; // specify non-zero if bound to a specific device
        // TBD-- we could cache the route and just check its validity
        route = ipv6->GetRoutingProtocol()->RouteOutput(p, header, oif, errno_);
        if (route)
        {
            NS_LOG_LOGIC("Route exists");
            header.SetSource(route->GetSource());
            m_udp->Send(p->Copy(),
                        header.GetSource(),
                        header.GetDestination(),
                        m_endPoint6->GetLocalPort(),
                        port,
                        route);
            NotifyDataSent(p->GetSize());
            return p->GetSize();
        }
        else
        {
            NS_LOG_LOGIC("No route to destination");
            NS_LOG_ERROR(errno_);
            m_errno = errno_;
            return -1;
        }
    }
    else
    {
        NS_LOG_ERROR("ERROR_NOROUTETOHOST");
        m_errno = ERROR_NOROUTETOHOST;
        return -1;
    }

    return 0;
}

// maximum message size for UDP broadcast is limited by MTU
// size of underlying link; we are not checking that now.
// \todo Check MTU size of underlying link
uint32_t
SCIONUdpSocketImpl::GetTxAvailable() const
{
    NS_LOG_FUNCTION(this); // requires g_log
    // No finite send buffer is modelled, but we must respect
    // the maximum size of an IP datagram (65535 bytes - headers).
    return MAX_IPV4_UDP_DATAGRAM_SIZE;
}



int
SCIONUdpSocketImpl::ShutdownRecv()
{
    NS_LOG_FUNCTION(this); // requires g_log
    m_shutdownRecv = true;
    if (m_endPoint)
    {
        m_endPoint->SetRxEnabled(false);
    }
    if (m_endPoint6)
    {
        m_endPoint6->SetRxEnabled(false);
    }
    return 0;
}


}
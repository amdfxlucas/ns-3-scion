#include "ns3/border-router-application.h"
#include "ns3/scion-as.h"
#include "ns3/ipv4.h"
#include "ns3/ipv6.h"
#include "ns3/scion-header.h"
#include "ns3/decoded-path.h"

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("BorderRouterApplication");

    void BorderRouterApplication::StopApplication()
    {

        // set callbacks to Null
        // close the sockets
        // goodbye
    }


    void BorderRouterApplication::OnRecvFromEndHost( Ptr<Socket> sock )
    {
      /*  // this method is a stub until i have implemented a RawSCIONSocket
          // that leaves the SCIONHeader in the packet
        auto routingState = GetNode()->GetObject<BorderRouterRoutingState>();
        Ptr<Packet> packet;
        Address from;
        while ( (packet = sock->Socket::RecvFrom(from) ) )
        {
            
        }
        */
    }    

    void BorderRouterApplication::OnRecvFromOtherBr( Ptr<Socket> sock )
    {
        //auto psock = DynamicCast<PacketSocket>(sock);
        auto localIFID = sock->GetBoundNetDevice()->GetIfIndex();
        

         auto routingState = GetNode()->GetObject<BorderRouterRoutingState>();
        Ptr<Packet> packet;
        Address from;
        while ( (packet = sock->Socket::RecvFrom(from) ) )
        {
            
            SCIONHeader scion;
            [[maybe_unused]] auto n = packet->PeekHeader(scion);

            if( scion.GetDstIA() == scion.GetSrcIA() )
            {
                return;
            }

            if( scion.GetDstIA() == routingState->m_localIA )
            {
//                DeliverPacketLocally(packet );
            }

            auto receivedFromLocalAS = routingState->m_inv_local[localIFID] == routingState->m_localAS;

            if( !receivedFromLocalAS )
            {
    //            PrepareInterDomainPacket( packet );
            }

            DecodedPath path = DecodedPath::ConvertFrom(scion.GetPath() );

            NS_ASSERT( path.GetCurrINF() >=0 );
            NS_ASSERT( path.GetCurrINF() < path.base.NumINF );


        }

    }

    void BorderRouterApplication::DetermineAndSetLocalListenAddress()
    {
        auto routingState = GetNode()->GetObject<BorderRouterRoutingState>();
        auto i4 = GetNode()->GetObject<Ipv4>();
        auto iif = i4->GetInterfaceForDevice( GetNode()->GetDevice( routingState->m_fstNonIP_IFIDx-1) );
        routingState->m_listenAddr = i4->GetAddress(iif,0).GetLocal(); // get the first address on this interface
    // oder sollte ich um ganz sicher zu sein hier das erste interface heraussuchen, wo isUp() true ist ..
    }

    void BorderRouterApplication::StartApplication()
    {
        
        auto routingState = GetNode()->GetObject<BorderRouterRoutingState>();
        // create the intra domain socket
       if( is_intra_domain_listener  )
       {        
       
        routingState->m_sock->SetRecvCallback( MakeCallback( &BorderRouterApplication::OnRecvFromEndHost ,this) );
       }

        m_inter_sock->SetRecvCallback( MakeCallback( &BorderRouterApplication::OnRecvFromOtherBr ,this) );
    
    }

  BorderRouterApplication::BorderRouterApplication()
  {

  }

/*
it is assumed that the BorderRouter's Node already got an IntraDomain Address 
before it is connected to neighboring border routers
*/
  BorderRouterApplication::  BorderRouterApplication(std::pair<ScionAs*,ASIFID_t> from,
                            std::pair<ScionAs*,ASIFID_t> to, 
                            Ptr<NetDevice> device,
                            IFIDx_t peerDeviceID )
    {
        auto routingState = GetNode()->GetObject<BorderRouterRoutingState>();
        if( !routingState ) // we are the first application to be constructed
        {
            is_intra_domain_listener = true;
            routingState = Create<BorderRouterRoutingState>();
            routingState->m_fstNonIP_IFIDx = device->GetIfIndex();
            GetNode()->AggregateObject(routingState);

            // find out the nodes address to listen on
            DetermineAndSetLocalListenAddress();
            routingState->m_sock = Create<SCIONUdpSocketImpl>();
            routingState->m_sock->Bind( routingState->m_listenAddr );
             // the listen port will be 0 this way ( if i recall correctly )

             routingState -> m_localAS = from.first->AS();
             routingState->m_localIA = from.first->IA();
        }
        routingState = GetNode()->GetObject<BorderRouterRoutingState>();

        

        routingState->m_connectivity.insert( to.first->AS(), device->GetIfIndex(), to.second ) ;

        routingState->m_local.emplace( from.second, device->GetIfIndex() );
        
        routingState->m_inv_local.emplace( device->GetIfIndex(), to.first->AS() );

        routingState->m_proper_local.emplace( std::pair<AS_t,ASIFID_t>{to.first->AS(),to.second}, device->GetIfIndex() );

        PacketSocketAddress local_addr;
        local_addr.SetSingleDevice( device->GetIfIndex() );
        local_addr.SetProtocol(0x0800);
        PacketSocketAddress peer_addr;
        peer_addr.SetSingleDevice( peerDeviceID );
        peer_addr.SetProtocol(0x0800);

        TypeId tid = TypeId::LookupByName("ns3::PacketSocketFactory");
        
        /* or even store the interDomainSockets in the routingState as well ?!
        */

        m_inter_sock = DynamicCast<PacketSocket>( Socket::CreateSocket(GetNode(), tid) );
        m_inter_sock -> Bind(local_addr);
        m_inter_sock->Connect( peer_addr );
        // silly because its a p2p link with only 2 devices, but now we can use send(packet)


        
    }

void BorderRouterRoutingState::connectivity_t::insert(AS_t as,IFIDx_t local_idx, ASIFID_t remote_as_ifid )
{
    // std::map< AS_t , std::set< std::pair< IFIDx_t, ASIFID_t> >  > m_connectivity;

    // already connected to that AS  ?
    if(! m_conn.contains(as) )
    {
        m_conn.emplace(as,  std::set< std::pair<IFIDx_t,ASIFID_t>>{ std::pair<IFIDx_t,ASIFID_t>{local_idx,remote_as_ifid} }  );
    }else
    {
        m_conn[as].emplace( local_idx,remote_as_ifid);
    }
    

    m_conn_out[as][remote_as_ifid] = local_idx;
}

}
/*
 * Copyright (c) 2014 Universita' di Firenze
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
 * Author: Tommaso Pecorella <tommaso.pecorella@unifi.it>
 */

// Network topology
//
//       n0              n1
//       |               |
//       =================
//         SimpleChannel
//
// - Packets flows from n0 to n1
//
// This example shows how to use the PacketSocketServer and PacketSocketClient
// to send non-IP packets over a SimpleNetDevice

#include "ns3/core-module.h"
#include "ns3/network-module.h"
//#include "ns3/simple-net-device-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/application.h"




using namespace ns3;

void ReceivePkt(std::string context ,Ptr<const Packet> packet, const Address& from)
{
    auto addr = PacketSocketAddress::ConvertFrom(from);
    std::cout << "context: " <<context<< " packet received from: "
    << addr.GetSingleDevice()<<" packet: "<< packet->ToString()<<std::endl;
    
}

 void sendThroughSocket( Ptr<PacketSocketServer> srv , Ptr<Packet> packet, int to_index )
 {  PacketSocketAddress addr;
     addr.SetSingleDevice( to_index);
    // socketAddr.SetPhysicalAddress(nodes.Get(0)->GetDevice(i)->GetAddress());
    addr.SetProtocol(0x0800);
     auto PacketSock = DynamicCast<PacketSocket>(srv->GetSocket());
                      PacketSock->SendTo(packet,0,addr);
}

int
main(int argc, char* argv[])
{
    bool verbose = true;

    CommandLine cmd(__FILE__);
    cmd.AddValue("verbose", "turn on log components", verbose);
    cmd.Parse(argc, argv);

    if (verbose)
    {
        LogComponentEnable("PacketSocketServer", LOG_LEVEL_ALL);
        LogComponentEnable("PacketSocketClient", LOG_LEVEL_ALL);
        LogComponentEnable("SimpleNetDevice", LOG_LEVEL_ALL);
    }
    // node index to map of <this-nodes-local-device-index
    // and remote-node-device-index of the device, this local device is connected with
    std::map< int, std::map<int,int> > link;

    using linkmap_t = std::map< /*fromNodeLocalIFID*/ int, std::pair</*remoteNodeID*/ int,int /*remoteNodeDeviceIFID*/> >;
    std::map< int /*fromNodeID*/,linkmap_t            > nodelink;

    using outmap_t =  std::map< int /*remoteNodeID*/,
    std::pair< /*remoteDeviceID*/ int, /*localDeviceID*/int > >;
     // a pair is sufficient here as this is not a multigraph
     // ( there is only one edge at most between any two nodes )
    std::map< /*fromNode*/ int, outmap_t > nodelinkout;

    // such a map is needed by Autonomous Systems, but not BorderRouters
    // remoteNodeID   -> remoteAS number
    // remoteDeviceID -> ASIFID_t Autonomous System ingress/egress interface ID of the remote(neighboring) AS
    // localDeviceId   -> ASIFID_t  Autonomous System ingress/egress interface ID
    // using outmapMulti_t = std::map< int /*remoteNodeID*/,  std::map<int /*remoteDeviceID*/, std::vector<int>/*localDeviceId*/> > ;

    NodeContainer nodes;
    nodes.Create(5);

    ns3::PacketMetadata::Enable();

    PacketSocketHelper packetSocket;

    // give packet socket powers to nodes.
    packetSocket.Install(nodes);

    //SimpleNetDeviceHelper helper;
    //helper.SetNetDevicePointToPointMode(true);

    PointToPointHelper helper;
    

    int k=0; // number of created links -> it must be (n over 2) = 10 by 5 nodes
    for( uint i = 0; i< nodes.GetN(); ++i)
    {
        // channels are bidirectional
        // so dont connect a pair (i,j) twice
        // and thus double their bandwidth
        for(uint j = 0; j<i; ++j)
        {
            if( i!= j)
            {
                k++;
               auto nc = helper.Install( nodes.Get(i),nodes.Get(j) );

               link[i].emplace( nc.Get(0)->GetIfIndex(), nc.Get(1)->GetIfIndex() );
               link[j].emplace( nc.Get(1)->GetIfIndex(), nc.Get(0)->GetIfIndex() );

               nodelink[i].emplace( nc.Get(0)->GetIfIndex(), std::pair<int,int>{j, nc.Get(1)->GetIfIndex()  } );
               nodelink[j].emplace(nc.Get(1)->GetIfIndex(), std::pair<int,int>{i,nc.Get(0)->GetIfIndex() } );

                nodelinkout[i].emplace( j, std::pair<int,int>{ nc.Get(1)->GetIfIndex(), nc.Get(0)->GetIfIndex() }  );
                nodelinkout[j].emplace( i, std::pair<int,int>{ nc.Get(0)->GetIfIndex(), nc.Get(1)->GetIfIndex() }  );
            }
        }
    }
    // now each node will have 4x netDevices
    // node i's device j will be connected to node 


    /*              i         j
         deviceIdx  nodeID   nodeID  deviceIdx
            0        0    -  1       0
            1        0    -  2       0
            2        0    -  3       0
            3        0    -  4       0
            1        1    -  2       1
            2        1    -  3       1
            3        1    -  4       1
            2        2    -  3       2
            3        2    -  4       2
            3        3    -  4       3

    
    */

    for (const auto& node : {0, 1, 2, 3, 4})
    {
        for (int i = 0; i < 4; ++i)
        {
            PacketSocketAddress socketAddr;
            socketAddr.SetSingleDevice(nodes.Get(node)->GetDevice(i)->GetIfIndex());
            // socketAddr.SetPhysicalAddress(nodes.Get(0)->GetDevice(i)->GetAddress());
            socketAddr.SetProtocol(0x0800);
            Ptr<PacketSocketServer> server = CreateObject<PacketSocketServer>();
            // server->TraceConnectWithoutContext("Rx", MakeCallback(&ReceivePkt)); //  "N"+
            // std::to_string(node)+"-A"+ std::to_string(i)+
            std::string contextString = "N" + std::to_string(node) + "-A" + std::to_string(i);
            // std::string contextString = std::to_string(nodes.Get(node)->GetId()) ;

            server->TraceConnect("Rx", contextString, MakeCallback(&ReceivePkt));
            server->SetLocal(socketAddr);
            nodes.Get(node)->AddApplication(server);
        }
    }

    PacketSocketAddress sockAddr;
    sockAddr.SetProtocol(0x0800);
   /// sockAddr.SetPhysicalAddress(nodes.Get(0)->GetDevice(0)->GetAddress());
    sockAddr.SetSingleDevice(nodes.Get(0)->GetDevice(0)->GetIfIndex());
    Ptr<PacketSocketClient> client = CreateObject<PacketSocketClient>();
    client->SetAttribute("PacketSize", UintegerValue(1000));
    client->SetAttribute("MaxPackets", UintegerValue(1));
    client->SetRemote(sockAddr);
    // nodes.Get(0)->AddApplication(client); // this does not work
    //nodes.Get(1)->AddApplication(client); // node 1 application 5
    nodes.Get(2)->AddApplication(client); // node 2 application 0

{
    auto packet = Create<Packet>(11);
    
    Ptr<PacketSocketServer> srv = DynamicCast<PacketSocketServer>(nodes.Get(0)->GetApplication(0));

Simulator::Schedule(Seconds(0.1) ,
sendThroughSocket,
srv,
packet,
nodes.Get(1)->GetDevice(0)->GetIfIndex() )  ; 
}

{
auto packet = Create<Packet>(13);

Ptr<PacketSocketServer> srv = DynamicCast<PacketSocketServer>(nodes.Get(1)->GetApplication(1) );

Simulator::Schedule(Seconds(0.1),
                    sendThroughSocket,
                    srv,
                    packet,
                    nodes.Get(2)->GetDevice(1)->GetIfIndex());
}

{
auto packet = Create<Packet>(15);

Ptr<PacketSocketServer> srv = DynamicCast<PacketSocketServer>(nodes.Get(1)->GetApplication(1) );

Simulator::Schedule(Seconds(0.1),
                    sendThroughSocket,
                    srv,
                    packet,
                    link[1][1] );
}


{
auto packet = Create<Packet>(17);

Ptr<PacketSocketServer> srv = DynamicCast<PacketSocketServer>(nodes.Get(2)->GetApplication(2) );

Simulator::Schedule(Seconds(0.1),
                    sendThroughSocket,
                    srv,
                    packet,
                    nodelink[2][2].second );
}

{
auto packet = Create<Packet>(19);

auto fromNode = 4;
auto toNode = 3;
//auto toNodeIFid = 1; // we can not choose this ( as this is no multigraph)



auto localNodeIf = nodelinkout[fromNode][toNode].second; // local node interface to reach the desired remote nodes interface

auto remoteDeviceID =nodelinkout[fromNode][toNode].first; // the remote device Id on the other side of the link

Ptr<PacketSocketServer> srv = DynamicCast<PacketSocketServer>(nodes.Get(fromNode)->GetApplication(localNodeIf) );
Simulator::Schedule(Seconds(0.1),
                    sendThroughSocket,
                    srv,
                    packet,
                    remoteDeviceID );
}

  /*  

    PacketSocketAddress socketAddr;
    socketAddr.SetSingleDevice(txDev->GetIfIndex());
    socketAddr.SetPhysicalAddress(rxDev->GetAddress());
    socketAddr.SetProtocol(1);

    Ptr<PacketSocketClient> client = CreateObject<PacketSocketClient>();
    client->SetRemote(socketAddr);
    nodes.Get(0)->AddApplication(client);

    Ptr<PacketSocketServer> server = CreateObject<PacketSocketServer>();
    server->SetLocal(socketAddr);
    nodes.Get(1)->AddApplication(server);
*/


/*auto sendThroughSocket = [&srv,&packet]( )
                    { auto PacketSock = DynamicCast<PacketSocket>(srv->GetSocket());
                      PacketSock->Send(packet,0);
                    };

  Simulator::Schedule(Seconds(0.1),   &decltype(sendThroughSocket)::operator(),sendThroughSocket )  ; // node 0's application 0 is connected to node 1's application 0 ?!
*/

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}

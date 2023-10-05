/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2022 ETH Zuerich
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
 * Author: Seyedali Tabaeiaghdaei seyedali.tabaeiaghdaei@inf.ethz.ch
 */

#ifndef SCION_SIMULATOR_SCION_AS_H
#define SCION_SIMULATOR_SCION_AS_H

#include "ns3/scion-types.h"
#include "beaconing/beacon.h"
#include "border-router.h"
#include "path-server.h"
#include "scion-host.h"
#include "scion-packet.h"
#include "user-defined-events.h"
#include "utils.h"

#include "ns3/map-scheduler.h"
#include "ns3/network-module.h"
#include "ns3/node.h"
#include "ns3/point-to-point-channel.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/point-to-point-net-device.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace YAML
{
class Node;
}

namespace ns3
{

class SCIONSimulationContext;

enum NeighbourRelation
{
    CORE = 0,
    PEER = 1,
    CUSTOMER = 2,
    PROVIDER = 3
};

class BeaconServer;
class BorderRouterApplication;

/*!
  \details  A ScionAS has a collection of BorderRouters, which connect it to other ASes.
            Also it owns all its resident ScionHosts.
            Each ScionAS has one BeaconServer and one PathServer.
            In ns-3 terms ScionASes are Nodes, which are connected through PointToPoint Links.
            A ScionAS has one NetDevice for each Link to a neighbor.
            

*/
class ScionAs
{
  public:
  // why do scionASes dont no their isd / IA ?!
  // but only an alias AS -> they do! They load it as a Property from the xml_node
    ScionAs(uint32_t system_id,
            bool parallel_scheduler,
            uint16_t as_number,
            rapidxml::xml_node<>* xml_node,
            const YAML::Node& config,
            bool malicious_border_routers,
            Time local_time);

    virtual ~ScionAs()
    {
    }
    auto AS() const {return as_number;}
    auto ISD() const {return isd_number;}
    auto IA()const{return ia_addr;}
    
    auto AllocateAsInterface(){ return m_interfaces++;}
    auto GetNInterfaces()const{return m_interfaces; }
    

    Time local_time;
    int32_t as_max_bwd;

    std::vector<Time> latencies_between_hosts_and_path_server;
    std::vector<Time> latencies_between_interfaces_and_beacon_server;
    Time latency_between_path_server_and_beacon_server;

    // collection of mappings from neighbor-AS-Nrs to the kind of neighbor-relation
    std::vector<std::pair<AS_t, NeighbourRelation>> neighbors;

    // if i want to reach my neighbor AS with AS-Nr 'X' I can go through each of map[ X ]'s of my local interfaces
    std::unordered_map<AS_t, std::vector< ASIFID_t> > interfaces_per_neighbor_as;

    // this AS's local interface if_i is connected to AS with number == map[ if_i ]
    std::unordered_map<ASIFID_t, AS_t> interface_to_neighbor_map;

    // geographical location of the interfaces to neighboring ASes
    // the AS-interface with ID 'i' has coordinates interfaces_coordinates[i]
    // Because interface IDs can share geographical locations, this list contains duplicates
    std::vector<std::pair<ld, ld>> interfaces_coordinates;
    
    // the inverse map of interfaces_coordinates
    std::multimap<std::pair<ld, ld>, ASIFID_t> coordinates_to_interfaces;
    
    // i-th entry is a vector that stores in its j-th position the latency between AS_i <-> AS_j
    std::vector<std::vector<ld>> latencies_between_interfaces;
    
    std::vector<int32_t> inter_as_bwds;

    void DoInitializations(uint32_t num_ases,
                           rapidxml::xml_node<>* xml_node,
                           const YAML::Node& config,
                           bool only_propagation_delay);

    void DoInitializations(uint32_t num_ases,
                           rapidxml::xml_node<>* xml_node,
                           const YAML::Node& config);

    std::pair<ASIFID_t, ScionAs*> GetRemoteAsInfo(ASIFID_t egress_interface_no);


    void SetBeaconServer(BeaconServer* beacon_server);

    void SetPathServer(PathServer* path_server);

    BeaconServer* GetBeaconServer();

    PathServer* GetPathServer();

    ScionCapableNode* GetHost(host_addr_t host_addr);

    uint32_t GetNHosts();

    void AdvanceTime(ns3::Time advance);

    void AddHost(ScionHost* host);

    BorderRouter* AddBr(double latitude,
                        double longitude,
                        Time processing_delay,
                        Time processing_throughput_delay);

   /* BorderRouterApplication* AddBorderRouterApplication( double latitude, double longitude,
                                                         Time processing_delay,
                                                          Time  processing_throughput_delay);    */                    

    void AddToRemoteAsInfo(uint16_t remote_if, ScionAs* remote_as);

    friend class UserDefinedEvents;


    // hier bloß ein paar brainstorming idden
    /*
    std::vector<Address> BorderRouterAddresses() const
    oder   std::vector<L4Address> um ports zu haben
    oder muss es std:: vector< std::pair< Address, IFID_t > > sein,
     damit zu jeder addresse das entsprechende egress Interface des AS mit angegeben ist
     (denn das ist ja letztendlich dass, was in den hopFields der pfade steht)

    die information über underlay addressen hat der pathserver nicht (im moment jedenfalls)
    Bleibt also vorerst nur das AS, dass diese information vorhalten muss
    um den SCIONHostContext zu construieren

     -------------------------------------------------------------
    SCIONHostContext& GetHostContext()   
    
    - HostContext enthält aufjedenfall pointer zum pathsever des AS
      und vielleicht auch pointer zum AS selber

    das AS scheint die geeignete Stelle um sich den ScionHostKontext abzuholen, 
    um ihn in SCIONNodes zu aggregieren

    z.B.: 
      NodeContainer  nodes;  // hier drinn sind die Bewohner des AS,
                            // die aus dem IntraDomainTopofile geladen wurden
       SCIONStackHelper  helper( pointer-to-AS )
      helper.Install(nodes);  // richtet auf allen nodes einen ScionHostContext passend zum AS ein
                            // und befähigt die nodes damit SCIONUdpSockets zu benutzen
                            // ( diese gebrauchen den HostContext um sich über den darin enthaltenen Daemon
                                mit pfaden zu versorgen , damit sie gültige SCIONHeader ausfüllen können )


    */

  protected:
    uint16_t m_interfaces=0;
    uint16_t isd_number;
    uint16_t as_number;
    ia_t ia_addr;

    bool malicious_border_routers;
    std::string border_routers_malicious_action;

    BeaconServer* beacon_server;
    PathServer* path_server = NULL;
    std::vector<ScionHost*> hosts;
    std::vector<BorderRouter*> border_routers;


    std::map< ASIFID_t , Node* > m_border_router_for_interface;

    std::vector<std::pair<ASIFID_t, ScionAs*> > remote_as_info;

    void ConnectInternalNodes(bool only_propagation_delay);
    void InitializeLatencies(bool only_propagation_delay);

    void InstantiateBeaconServer(bool parallel_scheduler,
                                 rapidxml::xml_node<>* xml_node,
                                 const YAML::Node& config);


  friend class SCIONSimulationContext;
};
} // namespace ns3
#endif // SCION_SIMULATOR_SCION_AS_H

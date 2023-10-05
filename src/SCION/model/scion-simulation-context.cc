#include "ns3/scion-simulation-context.h"

#include <yaml-cpp/yaml.h>
#include "ns3/nstime.h"

#include "ns3/pre-simulation-setup.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/string-utils.h"
#include "ns3/user-defined-events.h"
#include "ns3/point-to-point-module.h"
#include "ns3/topology-read-module.h"
#include "ns3/post-simulation-evaluations.h"
#include "ns3/border-router-application.h"

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("SCIONSimulationContext");

     SCIONSimulationContext*  SCIONSimulationContext::m_ctx;
      UserDefinedEvents*  SCIONSimulationContext::m_events;
      SCIONSimulationContext::GlobalScheduling*  SCIONSimulationContext::m_sched;
     PostSimulationEvaluations*  SCIONSimulationContext::m_posteval;

 /*   std::reference_wrapper<SCIONSimulationContext>  SCIONSimulationContext::m_ctx ;
    std::reference_wrapper<SCIONSimulationContext>  SCIONSimulationContext::m_events;
    std::reference_wrapper<SCIONSimulationContext>  SCIONSimulationContext::m_sched;
    std::reference_wrapper<SCIONSimulationContext>  SCIONSimulationContext:: m_posteval;
    */


SCIONSimulationContext& SCIONSimulationContext::FromConfig(const YAML::Node& cfg)
{
    static SCIONSimulationContext sim( &cfg );
    m_ctx = &sim;
    return *m_ctx;
}

 SCIONSimulationContext::SCIONSimulationContext(const YAML::Node* cfg )
 : config(*cfg)
 { 
 
 
    static UserDefinedEvents event( *this );
    m_events = &event;
    static GlobalScheduling sched(*this);
    m_sched = &sched;

    static PostSimulationEvaluations posteval(*this);
    m_posteval = &posteval;
 }

 PostSimulationEvaluations& 
 SCIONSimulationContext::Evaluations()
 {
   // return m_posteval.get();
   return *m_posteval;
 }

 SCIONSimulationContext::GlobalScheduling& SCIONSimulationContext::Scheduling()
    {
     //   static GlobalScheduling m_sched(getInstance());
     //   return m_sched.get();
     return *m_sched;
    }

void
SCIONSimulationContext::GetMaliciousTimeRefAndTimeServer(
                                 std::vector<std::string>& time_reference_types,
                                 std::vector<std::string>& time_server_types)
{
    std::vector<uint16_t> indices_time_references;
    std::vector<uint16_t> indices_time_servers;

    uint16_t number_of_ases = GetN();

    for (uint32_t i = 0; i < number_of_ases; ++i)
    {
        indices_time_references.push_back(i);
        indices_time_servers.push_back(i);
    }

    time_reference_types.resize(number_of_ases);
    time_server_types.resize(number_of_ases);

    if (config["time_service"]["truly_random_malicious"].as<uint16_t>() == 1)
    {
        std::shuffle(indices_time_references.begin(),
                     indices_time_references.end(),
                     std::random_device{});
        std::shuffle(indices_time_servers.begin(),
                     indices_time_servers.end(),
                     std::random_device{});
    }
    else
    {
        std::shuffle(indices_time_references.begin(),
                     indices_time_references.end(),
                     std::mt19937{});
        std::shuffle(indices_time_servers.begin(), indices_time_servers.end(), std::mt19937{});
    }

    uint16_t number_of_malicious_time_references = (uint16_t)std::floor(
        ((double)config["time_service"]["percent_of_malicious_time_references"].as<uint16_t>() *
         (double)number_of_ases) /
        100.0);

    uint16_t number_of_malicious_time_servers = (uint16_t)std::floor(
        ((double)config["time_service"]["percent_of_malicious_time_servers"].as<uint16_t>() *
         (double)number_of_ases) /
        100.0);

    if (config["time_service"]["reference_clk"].as<std::string>() == "OFF")
    {
        for (uint16_t i = 0; i < number_of_ases; ++i)
        {
            time_reference_types.at(i) = "OFF";
        }
    }
    else
    {
        for (uint16_t i = 0; i < number_of_malicious_time_references; ++i)
        {
            time_reference_types.at(indices_time_references.at(i)) = "MALICIOUS";
        }

        for (uint16_t i = number_of_malicious_time_references; i < number_of_ases; ++i)
        {
            time_reference_types.at(indices_time_references.at(i)) = "ON";
        }
    }

    for (uint16_t i = 0; i < number_of_malicious_time_servers; ++i)
    {
        time_server_types.at(indices_time_servers.at(i)) = "MALICIOUS";
    }

    for (uint16_t i = number_of_malicious_time_servers; i < number_of_ases; ++i)
    {
        time_server_types.at(indices_time_servers.at(i)) = "NORMAL";
    }
}

UserDefinedEvents& SCIONSimulationContext::Events()
{
    // static UserDefinedEvents m_events{ getInstance() };
   // return m_events.get();
   return *m_events;
}

void
SCIONSimulationContext::GetTimeServiceSnapShotTypes(                            
                            std::vector<std::string>& snapshot_types,
                            uint16_t& global_scheduler_and_printer)
{

    global_scheduler_and_printer = 0;
    if (config["time_service"]["snapshot_type"].as<std::string>() == "PRINT_OFFSET_DIFF")
    {
        std::random_device rd;
        std::uniform_int_distribution<uint16_t> dist(0, GetN() - 1);
        global_scheduler_and_printer = dist(rd);
    }
    for (uint32_t i = 0; i < GetN(); ++i)
    {
        if (config["time_service"]["snapshot_type"].as<std::string>() == "PRINT_OFFSET_DIFF")
        {
            if (i == global_scheduler_and_printer)
            {
                snapshot_types.push_back("PRINT_OFFSET_DIFF");
            }
            else
            {
                snapshot_types.push_back("OFF");
            }
        }
        else
        {
            snapshot_types.push_back(config["time_service"]["snapshot_type"].as<std::string>());
        }
    }
}

void
SCIONSimulationContext::GetTimeServiceAlgVersions(                        
                          std::vector<std::string>& alg_versions,
                          const std::vector<std::string>& snapshot_types)
{
    for (uint32_t i = 0; i < GetN(); ++i)
    {
        if (snapshot_types.at(i) == "OFF")
        {
            alg_versions.push_back(
                config["time_service"]["alg_version_non_printing_instances"].as<std::string>());
        }
        else
        {
            alg_versions.push_back(
                config["time_service"]["alg_version_printing_instances"].as<std::string>());
        }
    }
}

void
SCIONSimulationContext::InstantiateTimeServers()
{
    std::vector<std::string> time_reference_types;
    std::vector<std::string> time_server_types;
    std::vector<std::string> snapshot_types;
    std::vector<std::string> alg_versions;
    uint16_t global_scheduler_and_printer;

    GetMaliciousTimeRefAndTimeServer(time_reference_types, time_server_types);
    GetTimeServiceSnapShotTypes( snapshot_types, global_scheduler_and_printer);
    GetTimeServiceAlgVersions( alg_versions, snapshot_types);

    bool only_propagation_delay = OnlyPropagationDelay(config);

    for (uint32_t i = 0; i < GetN(); ++i)
    {
        auto as_node = nodes.at(i);
        uint16_t alias_as_no = as_node->AS();
        assert(alias_as_no == i);
        uint16_t isd_number = as_node->ISD();
        bool parallel_scheduler = (alias_as_no == global_scheduler_and_printer);

        ScionHost* time_server = new TimeServer(
            0,
            isd_number,
            alias_as_no,
            2,
            0.0,
            0.0,
            as_node.get(),
            parallel_scheduler,
            Time(config["time_service"]["max_initial_drift"].as<std::string>()),
            Time(config["time_service"]["max_drift_per_day"].as<std::string>()),
            config["time_service"]["jitter_in_drift"].as<uint32_t>(),
            config["time_service"]["max_drift_coefficient"].as<uint32_t>(),
            Time(config["time_service"]["global_cut_off"].as<std::string>()),
            Time(config["time_service"]["first_event"].as<std::string>()),
            Time(config["time_service"]["last_event"].as<std::string>()),
            Time(config["time_service"]["snapshot_period"].as<std::string>()),
            Time(config["time_service"]["list_of_ases_req_period"].as<std::string>()),
            Time(config["time_service"]["time_sync_period"].as<std::string>()),
            config["time_service"]["G"].as<uint32_t>(),
            config["time_service"]["number_of_paths_to_use_for_global_sync"].as<uint32_t>(),
            config["time_service"]["read_disjoint_paths"].as<std::string>(),
            config["time_service"]["time_service_output_path"].as<std::string>(),
            time_reference_types.at(alias_as_no),
            time_server_types.at(alias_as_no),
            snapshot_types.at(alias_as_no),
            alg_versions.at(alias_as_no),
            Time(config["time_service"]["malcious_response_minimum_offset"].as<std::string>()),
            config["time_service"]["path_selection"].as<std::string>());

        as_node->AddHost(time_server);

        if (only_propagation_delay)
        {
            time_server->SetProcessingDelay(Time(0), Time(0));
        }
        else
        {
            time_server->SetProcessingDelay(NanoSeconds(10), PicoSeconds(200));
        }
    }
}


    // this only instantiates a path server but doesnt connect it anywhere
void
SCIONSimulationContext::InstantiatePathServers()
{
    bool only_propagation_delay = OnlyPropagationDelay(config);

    for ( size_t i = 0; i < nodes.size(); ++i)
    {
        auto as_node = nodes.at(i);
        PathServer* path_server =
            new PathServer(0, as_node->ISD(), as_node->AS(), 1, 0.0, 0.0, as_node.get() );
        as_node->SetPathServer(path_server);

        if (only_propagation_delay)
        {
            path_server->SetProcessingDelay(Time(0), Time(0));
        }
        else
        {
            path_server->SetProcessingDelay(NanoSeconds(10), PicoSeconds(200));
        }
    }
}

/*!
  \param from pointer to From AS and from-AS-Interface-ID

  TODO: compute delay from greatCircleDistance

        maybe BorderRouter shouldnt be an application but
        just an Object aggregated into the node.
        Or even better, the BorderRouterApplication aggregates a
        RoutingState Object into its Node in its ctor
        if none it present already.
        This facilitates the Br ( node )to eventually take on more responsibility 
        in the curse of the simulation
        
*/
  void SCIONSimulationContext::InstallBorderRouterApplications( std::pair< ScionAs*, ASIFID_t> from,
                                         std::pair<ScionAs*,ASIFID_t> to,
                                         double latitude, double longitude ,
                                         bool only_prop_delay,
                                         uint32_t bwd )
{

auto from_topo = IntraASTopologyForAlias( from.first->AS() );
auto to_topo = IntraASTopologyForAlias( to.first->AS() );

auto next_from_br_id = from.first->m_border_router_for_interface.size();
auto from_br_to_be_id = from_topo->m_border_router_candidates[next_from_br_id];
auto from_br_to_be    = from_topo->m_nodes.Get(from_br_to_be_id);



auto next_to_br_id = to.first->m_border_router_for_interface.size();
auto to_br_to_be_id = to_topo->m_border_router_candidates[next_to_br_id];
auto to_br_to_be = to_topo->m_nodes.Get(to_br_to_be_id);


PointToPointHelper helper;
helper.SetChannelAttribute("Delay",StringValue("2ms"));
helper.SetDeviceAttribute("DataRate",IntegerValue(bwd) );//StringValue("5Mbps"));
auto devices = helper.Install(from_br_to_be,to_br_to_be);


auto from_br = Create<BorderRouterApplication>(from,to,devices.Get(0),devices.Get(1)->GetIfIndex());
from_br_to_be->AddApplication(from_br);


auto to_br = Create<BorderRouterApplication>(to,from,devices.Get(1),devices.Get(0)->GetIfIndex());

to_br_to_be->AddApplication(to_br);


from.first->m_border_router_for_interface.emplace( from.second, PeekPointer(from_br_to_be) );
to.first->m_border_router_for_interface.emplace(to.second, PeekPointer(to_br_to_be) );

}



/*!
  \details 
            installs one borderRouter for each link ( in AS from and AS to)
*/
void
SCIONSimulationContext::InstantiateLinksFromTopo(rapidxml::xml_node<>* xml_root                     
                       )
{
    bool only_propagation_delay = OnlyPropagationDelay(config);

    rapidxml::xml_node<>* curr_xml_node = xml_root->first_node("link");
    while (curr_xml_node)
    {
        int32_t to = std::stoi(curr_xml_node->first_node("to")->value());
        int32_t from = std::stoi(curr_xml_node->first_node("from")->value());

        PropertyContainer p = ParseProperties(curr_xml_node);

        ld latitude = std::stod(p.GetProperty("latitude"));
        ld longitude = std::stod(p.GetProperty("longitude"));
        int32_t bwd = std::stoi(p.GetProperty("capacity"));
        std::string rel = "core"; // p.GetProperty("rel");
        NeighbourRelation relation;

        // Check for the 3 possibilities in CAIDA topology
        if (rel == "peer")
        {
            relation = NeighbourRelation::PEER;
        }
        else if (rel == "core")
        {
            relation = NeighbourRelation::CORE;
        }
        else if (rel == "customer")
        {
            relation = NeighbourRelation::CUSTOMER;
        }
        else
        {
            relation = NeighbourRelation::CORE;
        }



        uint16_t to_alias_as_no = RealAsToAlias(to);
        uint16_t from_alias_as_no = RealAsToAlias(from);

    auto   to_as = nodes.at(to_alias_as_no);
auto        from_as =nodes.at(from_alias_as_no);

        assert(to_as->AS() == to_alias_as_no);
        assert(from_as->AS() == from_alias_as_no);

        /*
            Dont get confused. Each link from the topo file gets its own 
            designated AS-Interface-ID (with AllocateInterface)
            But it is possible that more than one link share a georaphical location  ()
            Or formulated differently: AS Interface IDs represented by ASIFID_t's
            denote the endpoints of links between ASes.
            Completely independent from the fact that these links might
            end in the same location and thus share a site-ID

        */
     
        auto from_if_id = from_as->AllocateAsInterface();
        auto to_if_id = to_as->AllocateAsInterface();

        to_as->AddToRemoteAsInfo(from_if_id, from_as.get() );

        to_as->interfaces_coordinates.push_back(std::pair<ld, ld>(latitude, longitude)); 

        NS_ASSERT_MSG(to_as->interfaces_coordinates.size() -1 == to_if_id, "coords.size(): " << to_as->interfaces_coordinates.size() << " to_if_id: " <<to_if_id );

 /*     this was a logic error! As more than one AS-interface share the same geoLocation,
        the interface id, got overridden each time and only the interface that was last encountered with this location was stored

        Luckyly this information was never used for anything
       to_as->coordinates_to_interfaces.insert(
            std::make_pair(std::pair<ld, ld>(latitude, longitude),
                           to_as->interfaces_coordinates.size() - 1));
  */

/* // some links are annotated by a python script
  p.HasProperty("border_router_site_id" )
*/


        if (p.HasProperty("to_if_id"))
        {
            assert((uint32_t)std::stoi(p.GetProperty("to_if_id")) == to_if_id );
        }

        from_as->AddToRemoteAsInfo( to_if_id, to_as.get() );

        from_as->interfaces_coordinates.push_back(std::pair<ld, ld>(latitude, longitude));

        /*from_as->coordinates_to_interfaces.insert(
            std::make_pair(std::pair<ld, ld>(latitude, longitude),
                           from_as->interfaces_coordinates.size() - 1));*/

        if (p.HasProperty("from_if_id"))
        {
            assert((uint32_t)std::stoi(p.GetProperty("from_if_id")) == from_if_id );
        }

        if (config["border_router"])
        {
           InstallBorderRouters( from_as.get(), to_as.get(),
                                 only_propagation_delay, latitude, longitude );
        }

        // this info is needed for the beaconServers to form the staticInfoExtension
        to_as->inter_as_bwds.push_back(bwd);
        from_as->inter_as_bwds.push_back(bwd);

        NeighbourRelation to_rel;
        NeighbourRelation from_rel;

        switch (relation)
        {
        case NeighbourRelation::PEER:
            to_rel = NeighbourRelation::PEER;
            from_rel = NeighbourRelation::PEER;
            break;
        case NeighbourRelation::CORE:
            to_rel = NeighbourRelation::CORE;
            from_rel = NeighbourRelation::CORE;
            break;
        case NeighbourRelation::CUSTOMER:
            to_rel = NeighbourRelation::PROVIDER;
            from_rel = NeighbourRelation::CUSTOMER;
            break;
        case NeighbourRelation::PROVIDER:
            // Should never happen, there is no "Provider" type in xml files
            to_rel = NeighbourRelation::CUSTOMER;
            from_rel = NeighbourRelation::PROVIDER;
            assert(false);
        }

        assert( to_if_id == to_as->GetNInterfaces() -1 ); // remove later
        to_as->interface_to_neighbor_map.insert(
            std::make_pair(to_if_id, from_as->AS()));

        if (to_as->interfaces_per_neighbor_as.find(from_as->AS()) !=
            to_as->interfaces_per_neighbor_as.end())
        {
            assert( to_if_id == to_as->GetNInterfaces() - 1 ); // remove later
            to_as->interfaces_per_neighbor_as.at(from_as->AS())
                .push_back((uint16_t)to_if_id );
        }
        else
        {
            assert( to_if_id == to_as->GetNInterfaces() - 1 ); // remove later
            std::vector<ASIFID_t> tmp;
            tmp.push_back((uint16_t)to_if_id );
            to_as->interfaces_per_neighbor_as.insert(std::make_pair(from_as->AS(), tmp));
            to_as->neighbors.push_back(std::make_pair(from_as->AS(), to_rel));
        }

        assert( from_if_id == from_as->GetNInterfaces() - 1 );// remove later

        from_as->interface_to_neighbor_map.insert(
            std::make_pair(from_if_id , to_as->AS()));

        if (from_as->interfaces_per_neighbor_as.find(to_as->AS()) !=
            from_as->interfaces_per_neighbor_as.end())
        {
            from_as->interfaces_per_neighbor_as.at(to_as->AS())
                .push_back(from_if_id);
        }
        else
        {
            std::vector<ASIFID_t> tmp;
            tmp.push_back((uint16_t)from_if_id);
            from_as->interfaces_per_neighbor_as.insert(std::make_pair(to_as->AS(), tmp));
            from_as->neighbors.push_back(std::make_pair(to_as->AS(), from_rel));
        }

        to_as->GetBeaconServer()->PerLinkInitializations(curr_xml_node, config); // currently No-Op
        from_as->GetBeaconServer()->PerLinkInitializations(curr_xml_node, config);

        curr_xml_node = curr_xml_node->next_sibling("link");
    }
}

void
SCIONSimulationContext::InstantiateASesFromTopo(rapidxml::xml_node<>* xml_root)
{
    uint16_t alias_as_no = 0;
    rapidxml::xml_node<>* cur_xml_node = xml_root->first_node("node");

    while (cur_xml_node)
    {
        PropertyContainer p = ParseProperties(cur_xml_node);
        std::string type;

        if (p.HasProperty("type"))
        {
            type = p.GetProperty("type");
        }
        else
        {
            type = "core";
        }

        bool malicious_border_routers = false;
        if (config["border_router"] && config["border_router"]["malicious_action"])
        {
            assert(p.HasProperty("malicious"));
            if (p.GetProperty("malicious") == "True")
            {
                malicious_border_routers = true;
            }
        }

        uint64_t real_as_no = std::stoi(GetAttribute(cur_xml_node, "id"));
        uint16_t isd_number = 0;
        if (p.HasProperty("isd"))
        {
            isd_number = std::stoi(p.GetProperty("isd"));
        }
        bool parallel_scheduler =  (alias_as_no == 0); // why treat one AS differently ?!
        std::shared_ptr<ScionAs> as_node;
        if (type == "core")
        {
            as_node = std::make_shared<ScionCoreAs>(0,
                                                   parallel_scheduler,
                                                    alias_as_no,
                                                    cur_xml_node,
                                                    config,
                                                    malicious_border_routers,
                                                    Time(0));
        }
        else if (type == "non-core")
        {
            as_node = std::make_shared<ScionAs>(0,
                                                parallel_scheduler,
                                                alias_as_no,
                                                cur_xml_node,
                                                config,
                                                malicious_border_routers,
                                                Time(0));
        }
        else
        {
            std::cerr << "Incompatible AS_node type!" << std::endl;
            exit(1);
        }
        // as_nodes.Add(as_node);
        AddScionAS(as_node, real_as_no, alias_as_no, isd_number);
        alias_as_no++;

        // for heavens sake , extract this into its own method !!
        if (p.HasProperty("Intra_AS_Topology"))
        {
            auto AsTopoFileInputName = p.GetProperty("Intra_AS_Topology");

            NS_ASSERT(p.HasProperty("Intra_AS_Topology_Format"));

            auto AsTopoFileFormat = p.GetProperty("Intra_AS_Topology_Format");

            if (AsTopoFileFormat == "Orbis")
            {
                // Pick a topology reader based in the requested format.
                TopologyReaderHelper topoHelp;
                topoHelp.SetFileName(AsTopoFileInputName);
                topoHelp.SetFileType(AsTopoFileFormat);
                Ptr<TopologyReader> inFile = topoHelp.GetTopologyReader();

                NS_ASSERT(p.HasProperty("Intra_AS_Protocol"));
                auto proto = p.GetProperty("Intra_AS_Protocol");

                std::shared_ptr<IntraASTopology> intra_as_topo;

                if (proto == "Ipv4")
                {
                    intra_as_topo = std::make_shared<Ipv4IntraASTopology>();
                }
                else if (proto == "Ipv6")
                {
                    intra_as_topo = std::make_shared<Ipv6IntraASTopology>();
                }
                else
                {
                    NS_ASSERT_MSG(false,
                                  "intra AS Protocol: " << proto
                                                        << " not implemented! [Ipv4|Ipv6]");
                }
                NS_ASSERT(p.HasProperty("EndHosts"));
                NS_ASSERT(p.HasProperty("BorderRouterCandidates"));

                
                     std::ranges::transform(
                         splitByDelim( p.GetProperty("EndHosts") , " ") ,
                      std::back_inserter(intra_as_topo->m_endHosts) ,
                      [](const std::string& s) -> uint16_t { return stoi(s); }
                      );                              
                     std::ranges::transform(
                         splitByDelim( p.GetProperty("BorderRouterCandidates") , " ") ,
                      std::back_inserter(intra_as_topo->m_border_router_candidates) ,
                      [](const std::string& s) -> uint16_t { return stoi(s); }
                      );    
               

                if (inFile)
                {
                    intra_as_topo->m_nodes = inFile->Read();
                }

                if (inFile->LinksSize() == 0)
                {
                    NS_LOG_ERROR("Problems reading the topology file. Failing.");
                }
                

                int totlinks = inFile->LinksSize();

                intra_as_topo->m_connected_node_pairs = std::make_shared<NodeContainer[]>(totlinks);
                TopologyReader::ConstLinksIterator iter;
                int i = 0;
                for (iter = inFile->LinksBegin(); iter != inFile->LinksEnd(); iter++, i++)
                {
                    intra_as_topo->m_connected_node_pairs[i] =
                        NodeContainer(iter->GetFromNode(), iter->GetToNode());
                }

               intra_as_topo->m_connected_net_devices_pairs =std::make_shared< NetDeviceContainer[]>(totlinks);
                PointToPointHelper p2p;
                for (int i = 0; i < totlinks; i++)
                {
                    // p2p.SetChannelAttribute ("Delay", TimeValue(MilliSeconds(weight[i])));
                    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
                    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
                    intra_as_topo->m_connected_net_devices_pairs[i] = p2p.Install(intra_as_topo->m_connected_node_pairs[i]);
                }


                InternetStackHelper stack; // this is proto agnostic (installs both ipv4/6)
                stack.Install(intra_as_topo->m_nodes);

                if(proto == "Ipv4")
                {
                    std::shared_ptr<Ipv4IntraASTopology> ipv4Topo = std::dynamic_pointer_cast<Ipv4IntraASTopology>( intra_as_topo );

                    NS_ASSERT(p.HasProperty("Intra_AS_SubnetMask"));
                    NS_ASSERT(p.HasProperty("Intra_AS_NetworkAddress"));

                Ipv4AddressHelper address;
                ipv4Topo->m_network_address = p.GetProperty("Intra_AS_NetworkAddress").c_str();
                ipv4Topo->m_netmask =  p.GetProperty("Intra_AS_SubnetMask") .c_str();
                address.SetBase(ipv4Topo->m_network_address,
                                ipv4Topo->m_netmask);
                
                //  auto ipic = new Ipv4InterfaceContainer[totlinks]; // i dont think its neccessary to keep em around
                for (int i = 0; i < totlinks; i++)
                {
                  //  ipic[i] = 
                  address.Assign(intra_as_topo->m_connected_net_devices_pairs [i]);
                }    

                AddIntraASTopoForAlias( alias_as_no, intra_as_topo );

                }
                else if ( proto == "Ipv6")
                {

                }
            }
            else
            {
                NS_ASSERT_MSG(false,
                              "no one has implemented " << AsTopoFileFormat
                                                        << " topo file parsing yet");
            }
        }

        cur_xml_node = cur_xml_node->next_sibling("node");
    }
}

void
SCIONSimulationContext::ReadBr2BrEnergy()
{
    std::ifstream energy_file(config["beacon_service"]["br_br_energy_file"].as<std::string>());
    std::string line;

    int counter = 0;
    while (getline(energy_file, line))
    {
        std::vector<std::string> fields;
        fields = Split(line, '\t', fields);

        int as_no = std::stoi(fields[0]);

        double lat1 = std::stod(fields[1]);
        double long1 = std::stod(fields[2]);

        double lat2 = std::stod(fields[3]);
        double long2 = std::stod(fields[4]);

        double energy = std::stod(fields[5]);

        if (real_to_alias_as_no.find(as_no) == real_to_alias_as_no.end())
        {
            counter++;
            continue;
        }

        uint16_t index = real_to_alias_as_no.at(as_no);
        auto as = nodes.at(index);
        NS_ASSERT(as->AS() == index);

        BeaconServer* beacon_server = as->GetBeaconServer();

        if (beacon_server->intra_as_energies.size() == 0)
        {
            beacon_server->intra_as_energies.resize(as->GetNInterfaces());
            for (uint32_t i = 0; i < as->GetNInterfaces(); ++i)
            {
                beacon_server->intra_as_energies.at(i).resize(as->GetNInterfaces());
            }
        }

        for (uint32_t i = 0; i < as->interfaces_coordinates.size(); ++i)
        {
            std::pair<double, double> coordinates1 = as->interfaces_coordinates.at(i);
            double if1_lat = coordinates1.first;
            double if1_long = coordinates1.second;

            if (std::abs(if1_lat - lat1) < 0.001 && std::abs(if1_long - long1) < 0.001)
            {
                for (uint32_t j = 0; j < as->interfaces_coordinates.size(); ++j)
                {
                    std::pair<double, double> coordinates2 = as->interfaces_coordinates.at(j);
                    double if2_lat = coordinates2.first;
                    double if2_long = coordinates2.second;

                    if (std::abs(if2_lat - lat2) < 0.001 && std::abs(if2_long - long2) < 0.001)
                    {
                        beacon_server->intra_as_energies.at(i).at(j) = energy;
                    }
                }
            }
        }
    }
    energy_file.close();
    std::cout << counter << std::endl;

    for (uint32_t i = 0; i < GetN(); ++i)
    {
       auto as = nodes.at(i);
        for (uint32_t j = 0; j < as->GetBeaconServer()->intra_as_energies.size(); ++j)
        {
            for (uint32_t k = 0; k < as->GetBeaconServer()->intra_as_energies.at(j).size(); ++k)
            {
                NS_ASSERT(as->GetBeaconServer()->intra_as_energies.at(j).at(k) != 0);
            }
        }
    }
}

void
SCIONSimulationContext::InitializeASesAttributes(                         
                         rapidxml::xml_node<>* xml_node                         )
{
    bool only_propagation_delay = OnlyPropagationDelay(config);

    if (config["border_router"])
    {
        for (uint64_t i = 0; i < GetN(); ++i)
        {
            auto as_node =nodes.at(i) ;
            as_node->DoInitializations(GetN(), xml_node, config, only_propagation_delay);
        }
    }
    else
    {
        for (uint64_t i = 0; i < GetN(); ++i)
        {
            auto as_node = nodes.at(i);
            as_node->DoInitializations( GetN(), xml_node, config);
        }
    }

    if (config["beacon_service"]["br_br_energy_file"])
    {
        ReadBr2BrEnergy( );
    }
}

}
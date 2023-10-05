#pragma once
#include <functional>
#include <map>
#include <memory>
#include <stdint.h>
#include <vector>
#include "ns3/net-device-container.h"
#include "ns3/node-container.h"
#include "ns3/rapidxml.hpp"
#include "ns3/scion-types.h"
#include "ns3/ipv4-address.h"

namespace YAML
{
class Node;
}

/*namespace rapidxml
{ template<class T>
  class xml_node;
};*/
namespace ns3
{
  class PostSimulationEvaluations;
class ScionAs;
    class UserDefinedEvents;
class SCIONSimulationContext
{
    friend class UserDefinedEvents;
  friend class PostSimulationEvaluations;
  public:
    



    class GlobalScheduling
    {
        SCIONSimulationContext& m_ctx;

        GlobalScheduling(SCIONSimulationContext& ctx)
            : m_ctx(ctx)
        {
        }
      friend class SCIONSimulationContext;
      public:
        void SchedulePeriodicEvents();

        void PeriodicBeaconingCheckPoint();

      private:
    };
    //class PostSimulationEvaluations;
    static PostSimulationEvaluations& Evaluations();

    static GlobalScheduling& Scheduling();

    static UserDefinedEvents& Events();

    static SCIONSimulationContext& FromConfig(const YAML::Node& cfg);

    static SCIONSimulationContext& getInstance()
    {
        return *m_ctx;
    }

    auto GetN() const
    {
        return nodes.size();
    }
    auto AsToIsd( auto as )const {return as_to_isd_map.at(as); }
    auto AsAliasToReal( auto alias)const{return alias_to_real_as_no.at(alias);}
    auto RealAsToAlias( auto real_as_nr)const{return real_to_alias_as_no.at(real_as_nr); }
   bool HasAliasForRealAS( auto realAsNr )const{return real_to_alias_as_no.find(realAsNr)!=real_to_alias_as_no.end();}

    void InstantiateTimeServers();

    void InstantiateLinksFromTopo(rapidxml::xml_node<>* xml_root);

    void InitializeASesAttributes(rapidxml::xml_node<>* xml_node);

    void InstantiateASesFromTopo(rapidxml::xml_node<>* xml_root);

    void InstantiatePathServers();
    auto NumCores()const{return num_core;} // vielleicht hier noch eine klasse Config/Params für so was allgemeines
                                          // auch die ganzen macros, wie z.b. BEACON_KEY_WIDTH ließen sich somit wirksam bekämpfen
    void SetNumCores(auto n){num_core = n;}
    std::vector<std::shared_ptr<ScionAs>>& Nodes() {return nodes;}
  private:


  void InstallBorderRouterApplications( std::pair< ScionAs*, ASIFID_t> from,
                                         std::pair<ScionAs*,ASIFID_t> to,
                                         double latitude, double longitude,
                                         bool only_prop_delay,
                                         uint32_t bandwidth);

  struct IntraASTopology
  {
    NodeContainer m_nodes; // the nodes read from the intra-AS topo file
    uint m_protoTypeID; // eigther ipv4/6 GetTypeID()


    // the index i into  these containers corresponds to the i-th intra-AS link from the topo file
    std::shared_ptr< NodeContainer[]> m_connected_node_pairs;
    std::shared_ptr< NetDeviceContainer[]> m_connected_net_devices_pairs;
    std::vector< uint16_t > m_border_router_candidates;
    // node IDs of core routers , sorted in ascending order according to edge-count to non-core nodes
    std::vector< uint16_t> m_endHosts;
    // node ids of nodes with only one edge
   virtual ~IntraASTopology(){} // only so that std::dynamic_pointer_cast works
  };

  struct Ipv4IntraASTopology : public IntraASTopology
  {
    // std::shared_ptr< Ipv4InterfaceContainer[] > m_connected_node_pairs_interfaces;
    Ipv4Address m_network_address;
    Ipv4Mask m_netmask;
  };

  struct Ipv6IntraASTopology : public IntraASTopology
  {
  };


// helper for initialiseASAttributes()
    void ReadBr2BrEnergy();

    // helper for InstantiateASesFromTopo()
    void AddScionAS(std::shared_ptr<ScionAs> newAS,
                    uint64_t real_as_no,
                    uint16_t alias_as_no,
                    uint16_t isd_number)
    {
        nodes.push_back(newAS);
        as_to_isd_map.emplace(alias_as_no, isd_number);

        real_to_alias_as_no.emplace(real_as_no, alias_as_no);
        alias_to_real_as_no.emplace(alias_as_no, real_as_no);
    }

    // helper for InstantiateTimeServers
    void GetMaliciousTimeRefAndTimeServer(std::vector<std::string>& time_reference_types,
                                          std::vector<std::string>& time_server_types);
    // helper for InstantiateTimeServers
    void GetTimeServiceSnapShotTypes(std::vector<std::string>& snapshot_types,
                                     uint16_t& global_scheduler_and_printer);
    // helper for InstantiateTimeServers
    void GetTimeServiceAlgVersions(  std::vector<std::string>& alg_versions,
        const std::vector<std::string>& snapshot_types);

    //  GlobalScheduling m_globalScheduling;

    const YAML::Node& config;

    SCIONSimulationContext(const YAML::Node* cfg);

std::shared_ptr<IntraASTopology> IntraASTopologyForAlias( uint16_t as_alias )
{ return m_intraASTopoForAlias.at( as_alias);
}

void AddIntraASTopoForAlias(  uint16_t as_alias, std::shared_ptr<IntraASTopology> topo )
{
  m_intraASTopoForAlias.emplace(as_alias,topo);
}

    // std::reference_wrapper<SCIONSimulationContext> m_ctx;

    static SCIONSimulationContext* m_ctx;
    static UserDefinedEvents* m_events;
    static GlobalScheduling* m_sched;
    static PostSimulationEvaluations* m_posteval;

    //std::reference_wrapper<SCIONSimulationContext::UserDefinedEvents> m_events;
    //std::reference_wrapper<GlobalScheduling> m_sched;
    //std::reference_wrapper<PostSimulationEvaluations> m_posteval;

    std::map<int32_t, uint16_t> real_to_alias_as_no;
    std::map<uint16_t, int32_t> alias_to_real_as_no;
    std::map<uint16_t, uint16_t> as_to_isd_map;
    uint32_t num_core;

    std::vector<std::shared_ptr<ScionAs>> nodes;


    std::map< uint16_t, std::shared_ptr<IntraASTopology> > m_intraASTopoForAlias;
    // maps an AS alias to its intra-AS-topology
};
} // namespace ns3
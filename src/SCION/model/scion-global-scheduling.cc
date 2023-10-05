#include "ns3/scion-simulation-context.h"

// #include "schedule-periodic-events.h"

#include "beaconing/beacon-server.h"
#include "scion-as.h"
#include "scion-host.h"
#include "time-server.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/nstime.h"
#include "ns3/point-to-point-helper.h"
//#include "ns3/ptr.h"
#include "ns3/simulator.h"
#include "scion-as.h"
#include "scion-packet.h"

#include <yaml-cpp/yaml.h>

#include <chrono>
#include <omp.h>

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("GlobalScheduling");
void
SCIONSimulationContext::GlobalScheduling::SchedulePeriodicEvents()
{
    for (uint32_t i = 0; i < m_ctx.GetN(); ++i)
    {
        auto node = m_ctx.nodes.at(i);

        if (m_ctx.config["beacon_service"])
        {
            node->GetBeaconServer()->ScheduleBeaconing(
                Time(m_ctx.config["beacon_service"]["last_beaconing"].as<std::string>()));
        }

        if (m_ctx.config["time_service"])
        {
            dynamic_cast<TimeServer*>(node->GetHost(2))->ScheduleListOfAllASesRequest();
            dynamic_cast<TimeServer*>(node->GetHost(2))->ScheduleSnapShots();
            dynamic_cast<TimeServer*>(node->GetHost(2))->ScheduleTimeSync();
        }
    }

    if (m_ctx.config["beacon_service"])
    {
        for (Time t = Seconds(0.0);
             t <= Time(m_ctx.config["beacon_service"]["last_beaconing"].as<std::string>());
             t += Time( m_ctx.config["beacon_service"]["period"].as<std::string>()))
        {
            Simulator::Schedule(t + TimeStep(2), &SCIONSimulationContext::GlobalScheduling::PeriodicBeaconingCheckPoint,this);
        }
    }
}

void
SCIONSimulationContext::GlobalScheduling::PeriodicBeaconingCheckPoint()
{
    std::cout << "################################## "
              << m_ctx.nodes.at(0)->GetBeaconServer()->GetCurrentTime()
              << " #########################################" << std::endl;
    uint32_t node_number = m_ctx.GetN();

    // print number of connected pairs after each beaconing round
    uint32_t all_connected_pairs = 0;
    for (uint32_t i = 0; i < node_number; ++i)
    {
        all_connected_pairs +=m_ctx.nodes.at(i)
                                   ->GetBeaconServer()
                                   ->GetValidBeaconsCountPerDstAs()
                                   .size();
    }
    std::cout << all_connected_pairs << std::endl;
}
}
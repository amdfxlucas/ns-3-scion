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

#ifndef SCION_SIMULATOR_POST_SIMULATION_EVALUATIONS_H
#define SCION_SIMULATOR_POST_SIMULATION_EVALUATIONS_H

#include "beaconing/beacon.h"
#include "scion-as.h"
#include "ns3/scion-simulation-context.h"

#include <yaml-cpp/yaml.h>

namespace ns3
{
class SCIONSimulationContext;

#define REGISTER_FUN(FUNC_NAME)                                                                    \
    function_name_to_function.insert(                                                              \
        std::make_pair(#FUNC_NAME, &PostSimulationEvaluations::FUNC_NAME));

class PostSimulationEvaluations
{
  public:
    PostSimulationEvaluations( SCIONSimulationContext& ctx );

    void DoFinalEvaluations();

    void PrintTrafficSentFromCollectorsPerDstPerPeriod();

    void PrintAllDiscoveredPaths();

    void PrintAllPathsAttributes();

    void PrintDistributionOfPathsWithSpecificHopCount();

    void PrintNoBeaconsPerInterface();

    void PrintNoBeaconsPerInterfacePerDstOrOpt();

    void PrintConsumedBwAtEachPeriod();

   // void PrintPathQualities();

 //   void EvaluateSTConnectivity();

    void PrintMinimumLatencyDist();

    void PrintPathNoDistribution();

    void FindMinLatencyToDnsRootServers();

    void PrintPathPollutionIndex();

    void PrintLeastPollutingPaths();

    void PrintBestPerHopPollutionIndexes();

   //  void PrintTransitTrafficBaseline();

    void InvestigateAffectedTimeServers();

    void PrintConsumedBwForBeaconing();

    void PrintBeaconStores();

    void PrintNumberOfValidBeaconEntriesInBeaconStore();

    friend void SortBeaconsByPollutionByLatency(
        ScionAs* as1,
        ScionAs* as2,
        std::string beaconing_policy_str,
        std::map<double, std::map<double, std::set<Beacon*>>>&
            sorted_beacons_by_pollution_by_latency);

  private:
    SCIONSimulationContext& m_ctx;
    Time beaconing_period;
    Time last_beaconing_event_time;
    Time first_beaconing;

    uint16_t expiration_period;
    std::string beaconing_policy_str;

    std::unordered_map<std::string, void (PostSimulationEvaluations::*)()>
        function_name_to_function;
};

void SortBeaconsByPollutionByLatency(
    ScionAs* as1,
    ScionAs* as2,
    std::string beaconing_policy_str,
    std::map<double, std::map<double, std::set<Beacon*>>>& sorted_beacons_by_pollution_by_latency);

} // namespace ns3
#endif // SCION_SIMULATOR_POST_SIMULATION_EVALUATIONS_H

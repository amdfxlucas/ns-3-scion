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
 * Author: Seyedali Tabaeiaghdaei seyedali.tabaeiaghdaei@inf.ethz.ch,
 *         Christelle Gloor  christelle.gloor@inf.ethz.ch
 */

#include "beacon-server.h"

#include "ns3/path-server.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/run-parallel-events.h"
#include "ns3/scion-core-as.h"
#include "ns3/string-utils.h"
#include "ns3/utils.h"


#include <omp.h>

namespace ns3
{

BeaconServer::BeaconServer(ScionAs* as,
                           bool parallel_scheduler,
                           rapidxml::xml_node<>* xml_node,
                           const YAML::Node& config)
    : 
      parallel_scheduler(parallel_scheduler),
      beaconing_period(Time(config["beacon_service"]["period"].as<std::string>())),
      expiration_period(Time(config["beacon_service"]["expiration_period"].as<std::string>())
                            .ToInteger(Time::MIN)),
      last_beaconing_event_time(Time(config["beacon_service"]["last_beaconing"].as<std::string>())),
      as(as)

{
    non_requested_pull_based_beacon_container.resize(2);
    PropertyContainer p = ParseProperties(xml_node);
    if (p.HasProperty("dirty_energy_ratio"))
    {
        dirty_energy_ratio = std::stod(p.GetProperty("dirty_energy_ratio"));
    }

    if (p.HasProperty("sun_energy_ratio"))
    {
        sun_energy_ratio = std::stod(p.GetProperty("sun_energy_ratio"));
    }

    if (config["beacon_service"]["read_beacons_directory"])
    {
        file_to_read_beacons =
            config["beacon_service"]["read_beacons_directory"].as<std::string>() + "beacons_" +
            std::to_string(as->AS()) + ".json";
    }
    else
    {
        file_to_read_beacons = "none";
    }

    if (config["beacon_service"]["write_beacons_directory"])
    {
        file_to_write_beacons =
            config["beacon_service"]["write_beacons_directory"].as<std::string>() + "beacons_" +
            std::to_string(GetAs()->AS()) + ".json";
    }
    else
    {
        file_to_write_beacons = "none";
    }
}

void
BeaconServer::DoInitializations(uint32_t num_ases,
                                rapidxml::xml_node<>* xml_node,
                                const YAML::Node& config)
{
    beacons_sent_per_interface.resize( GetAs()->GetNInterfaces());
    pull_based_beacons_sent_per_opt_per_interface.resize(GetAs()->GetNInterfaces());
    push_based_beacons_sent_per_opt_per_interface.resize(GetAs()->GetNInterfaces());
    beacons_sent_per_dst_per_interface.resize(GetAs()->GetNInterfaces());
}

void BeaconServer::PerLinkInitializations(rapidxml::xml_node<>* xml_node,
                                          const YAML::Node& config){};

void
BeaconServer::SetAs(ScionAs* as)
{
    this->as = as;
}

void
BeaconServer::ScheduleBeaconing(Time last_beaconing_event_time)
{
    if (parallel_scheduler)
    {
        Simulator::Schedule(Seconds(0),
                            &RunParallelEvents<void (BeaconServer::*)()>,
                            &BeaconServer::ReadBeacons);
        Simulator::Schedule(last_beaconing_event_time + TimeStep(2),
                            &RunParallelEvents<void (BeaconServer::*)()>,
                            &BeaconServer::WriteBeacons);
    }
    for (Time t = Seconds(0); t <= last_beaconing_event_time; t += beaconing_period)
    {
        if (parallel_scheduler)
        {
            if (GetAs()->GetPathServer() != NULL)
            {
                Simulator::Schedule(t + GetAs()->latency_between_path_server_and_beacon_server,
                                    &RunParallelEvents<void (BeaconServer::*)()>,
                                    &BeaconServer::RegisterToLocalPathServer);
            }
            Simulator::Schedule(t,
                                &RunParallelEvents<void (BeaconServer::*)()>,
                                &BeaconServer::UpdateStateBeforeBeaconing);

            Simulator::Schedule(t + TimeStep(1),
                                &RunParallelEvents<void (BeaconServer::*)()>,
                                &BeaconServer::UpdateStatePeriodic);
        }

        if (dynamic_cast<ScionCoreAs*>(GetAs()) != NULL)
        {
            Simulator::Schedule(t,
                                &BeaconServer::DisseminateBeacons,
                                this,
                                NeighbourRelation::CORE);

            Simulator::Schedule(t, &BeaconServer::InitiateBeacons, this, NeighbourRelation::CORE);
            Simulator::Schedule(t,
                                &BeaconServer::InitiateBeacons,
                                this,
                                NeighbourRelation::CUSTOMER);
        }
        else // we are BeaconServer of NonCoreAS
        {
            // thus we do no core beaconing
            Simulator::Schedule(t,
                                &BeaconServer::DisseminateBeacons,
                                this,
                                NeighbourRelation::CUSTOMER);
        }
    }
}

void
BeaconServer::InsertPulledBeaconsToBeaconStore()
{
    for (auto& key_beacon_pair : requested_pull_based_beacon_container)
    {
        Beacon* to_insert_beacon = &key_beacon_pair.second;
        std::vector<uint64_t>& the_path = to_insert_beacon->the_path;
        uint16_t dst_as = to_insert_beacon->optimization_target->target_as;

        std::reverse(the_path.begin(), the_path.end());
        for (uint32_t i = 0; i < the_path.size(); ++i)
        {
            the_path.at(i) = (the_path.at(i) >> 32) | (the_path.at(i) << 32);
        }

        NS_ASSERT(dst_as == UPPER_16_BITS(the_path.front()));

        uint16_t path_len = (uint16_t)to_insert_beacon->the_path.size();

        if (beacon_store.find(dst_as) != beacon_store.end() &&
            beacon_store.at(dst_as).find(path_len) != beacon_store.at(dst_as).end())
        {
            beacon_store.at(dst_as).at(path_len).insert(to_insert_beacon);
        }
        else if (beacon_store.find(dst_as) != beacon_store.end() &&
                 beacon_store.at(dst_as).find(path_len) == beacon_store.at(dst_as).end())
        {
            beacon_store.at(dst_as).insert(std::make_pair(path_len, beacons_with_equal_length()));
            beacon_store.at(dst_as).at(path_len).insert(to_insert_beacon);
        }
        else
        {
            beacon_store.insert(std::make_pair(dst_as, beacons_with_same_dst_as()));
            beacon_store.at(dst_as).insert(std::make_pair(path_len, beacons_with_equal_length()));
            beacon_store.at(dst_as).at(path_len).insert(to_insert_beacon);
        }
    }
}

void
BeaconServer::UpdateBeaconState(Beacon* the_beacon)
{
    uint16_t dst_as = DST_AS_PTR(the_beacon);
    NS_ASSERT(the_beacon->beacon_direction == BeaconDirectionT::PULL_BASED ||
              the_beacon->optimization_target == NULL ||
              ORIGINATOR_PTR(the_beacon) == the_beacon->optimization_target->target_as);
    if (the_beacon->is_new)
    {
        the_beacon->is_new = false;
        if (the_beacon->next_expiration_time > now)
        {
            if (!the_beacon->is_valid)
            {
                the_beacon->is_valid = true;
                IncrementValidBeaconsCount(dst_as);
            }
            the_beacon->initiation_time = the_beacon->next_initiation_time;
            the_beacon->expiration_time = the_beacon->next_expiration_time;
        }
    }

    if (the_beacon->expiration_time <= next_period && the_beacon->is_valid)
    {
        the_beacon->is_valid = false;
        DecrementValidBeaconsCount(dst_as);
        DecrementNextRoundValidBeaconsCount(dst_as);
    }
}

void
BeaconServer::CreateInitialStaticInfoExtension(static_info_extension_t& static_info_extension,
                                               uint16_t self_egress_if_no,
                                               const OptimizationTarget* optimization_target)
{
}

void
BeaconServer::InitiateBeacons(NeighbourRelation relation)
{
    NS_ASSERT(now == (uint16_t)Simulator::Now().ToInteger(Time::MIN));
    uint32_t neighbors_cnt = GetAs()->neighbors.size();
    omp_set_num_threads(SCIONSimulationContext::getInstance().NumCores());

#pragma omp parallel for
    for (uint32_t i = 0; i < neighbors_cnt; ++i)
    {
        if (GetAs()->neighbors.at(i).second != relation)
        {
            continue;
        }

        uint16_t remote_as_no = GetAs()->neighbors.at(i).first;
        const auto& interfaces = GetAs()->interfaces_per_neighbor_as.at(remote_as_no);
        for (const auto& self_egress_if_no : interfaces)
        {
            std::pair<uint16_t, ScionAs*> remote_as_if_pair =
                GetAs()->GetRemoteAsInfo(self_egress_if_no);

            uint16_t remote_ingress_if_no = remote_as_if_pair.first;
            ScionAs* remote_as = remote_as_if_pair.second;

            InitiateBeaconsPerInterface(self_egress_if_no, remote_as, remote_ingress_if_no);
        }
    }
}

void
BeaconServer::InitiateBeaconsPerInterface(uint16_t self_egress_if_no,
                                          ScionAs* remote_as,
                                          uint16_t remote_ingress_if_no)
{
    static_info_extension_t static_info_extension;
    CreateInitialStaticInfoExtension(static_info_extension, self_egress_if_no, NULL);

    GenerateBeaconAndSend(NULL,
                          self_egress_if_no,
                          remote_ingress_if_no,
                          remote_as,
                          static_info_extension);
}

/*! 
  \param  selected_beacon  a beacon, which was received earlier and shall be propagated to remote_as
  \param self_egress_if_no    the interface on which we send the beacon,
                 in order for it to reach sender_as's remote_ingress interface
  \param remote_ingress_if_no  the interface on which remote_as will receive the beacon
  \param remote_as  the AS to which we are about to forward the beacon
  \param beacon_direction
  \param optimization_target
  \param static_info_extension
*/
void
BeaconServer::GenerateBeaconAndSend(Beacon* selected_beacon,
                                    uint16_t self_egress_if_no,
                                    uint16_t remote_ingress_if_no,
                                    ScionAs* remote_as,
                                    static_info_extension_t& static_info_extension,
                                    const OptimizationTarget* optimization_target,
                                    BeaconDirectionT beacon_direction)
{
    std::string key;
    uint16_t remote_as_no = remote_as->AS();

    path new_path;
    isd_path new_isd_path;
    uint16_t next_initiation_time;
    uint16_t next_expiration_time;

    uint64_t link_info;
    link_info = (((uint64_t)GetAs()->AS()) << 48) | (((uint64_t)self_egress_if_no) << 32) |
                (((uint64_t)remote_as_no) << 16) | ((uint64_t)remote_ingress_if_no);

    if (selected_beacon == NULL)
    {
        next_initiation_time = now;
        next_expiration_time = now + expiration_period;
        if (optimization_target != NULL)
        {   
            //key = std::string((char*)&optimization_target->target_id, 2);
            key = toNdigit_string( optimization_target->target_id, BEACON_KEY_WIDTH );
            if (beacon_direction == BeaconDirectionT::PULL_BASED)
            {
                NS_ASSERT(optimization_target->target_as != remote_as_no);
                key = key + toNdigit_string(optimization_target->target_as, BEACON_KEY_WIDTH );
            }
        }
    }
    else
    {
        next_initiation_time = selected_beacon->initiation_time;
        next_expiration_time = selected_beacon->expiration_time;
        new_path = selected_beacon->the_path;
        key = selected_beacon->key;
        new_isd_path = selected_beacon->the_isd_path;
    }
    //  this conversion is just horrifying 
    key = key + toNdigit_string(GetAs()->AS(),BEACON_KEY_WIDTH)  +
           toNdigit_string(self_egress_if_no,BEACON_KEY_WIDTH) ;
    new_path.push_back(link_info);

    if (new_isd_path.size() == 0 || new_isd_path.back() != GetAs()->ISD())
    {
        new_isd_path.push_back(GetAs()->ISD());
    }

    uint16_t initiation_time =
        (beacon_direction == BeaconDirectionT::PULL_BASED) ? next_initiation_time : 0;
    uint16_t expiration_time =
        (beacon_direction == BeaconDirectionT::PULL_BASED) ? next_expiration_time : 0;

    Beacon to_disseminate_beacon(static_info_extension,
                                 optimization_target,
                                 beacon_direction,
                                 initiation_time,
                                 expiration_time,
                                 next_initiation_time,
                                 next_expiration_time,
                                 true,
                                 false,
                                 new_path,
                                 key,
                                 new_isd_path);

    IncrementControlPlaneBytesSent(to_disseminate_beacon, self_egress_if_no);
    remote_as->GetBeaconServer()->ReceiveBeacon(to_disseminate_beacon,
                             GetAs()->AS(),
                             self_egress_if_no,
                             remote_ingress_if_no);
}

void
BeaconServer::UpdateStatePeriodic()
{
    pull_based_read = (pull_based_read + 1) % 2;
    pull_based_write = (pull_based_write + 1) % 2;
    std::map<int32_t, std::unordered_map<std::string, Beacon>&> beacon_containers = {
        {1, push_based_beacon_container},
        {2, requested_pull_based_beacon_container}};

    for (const auto& beacon_container : beacon_containers)
    {
        for (auto& the_beacon_pair : beacon_container.second)
        {
            Beacon* the_beacon = &the_beacon_pair.second;

            bool was_valid = the_beacon->is_valid;
            UpdateBeaconState(the_beacon);
            bool is_valid = the_beacon->is_valid;
            bool invalidated = was_valid && (!is_valid);

            UpdateAlgorithmDataStructuresPeriodic(the_beacon, invalidated);
        }
    }
}

/*!
  \param sender_as  AS that forwarded the beacon to us ( last hop field on path ?!)
        In general this might be a different AS than the originator of the beacon [ DST_AS(received_becon)]
        which constructed and disseminated it in the first place.
   \param remote_if  The interface of sender_as, from which it forwarded the beacon to us (local_if)
   \param local_if  The interface on which we received the beacon from sender_as     
  

*/
void
BeaconServer::InsertBeacon(Beacon& the_beacon,
                           uint16_t dst_as,
                           uint16_t sender_as,
                           uint16_t remote_egress_if,
                           uint16_t local_ingress_if,
                           bool path_exists,
                           bool existing_path_valid,
                           Beacon* beacon_to_replace)
{
    if (the_beacon.beacon_direction == BeaconDirectionT::PULL_BASED)
    {
        auto& beacon_container =
            (ORIGINATOR(the_beacon) == GetAs()->AS())
                ? requested_pull_based_beacon_container
                : non_requested_pull_based_beacon_container.at(pull_based_write);
        NS_ASSERT(beacon_container.find(the_beacon.key) == beacon_container.end());
        beacon_container.insert(std::make_pair(the_beacon.key, the_beacon));
        Beacon* to_insert_beacon = &beacon_container.at(the_beacon.key);

        if (ORIGINATOR(the_beacon) == GetAs()->AS())
        {
            IncrementNextRoundValidBeaconsCount(dst_as);
        }

        InsertToAlgorithmDataStructures(to_insert_beacon,
                                        sender_as,
                                        remote_egress_if,
                                        local_ingress_if);
        return;
    }

    if (path_exists)
    {
        beacon_to_replace->next_initiation_time = the_beacon.next_initiation_time;
        beacon_to_replace->next_expiration_time = the_beacon.next_expiration_time;
        beacon_to_replace->is_new = true;

        if (!existing_path_valid)
        {
            IncrementNextRoundValidBeaconsCount(dst_as);
            InsertToAlgorithmDataStructures(beacon_to_replace,
                                            sender_as,
                                            remote_egress_if,
                                            local_ingress_if);
        }
        return;
    }
    else
    {
        IncrementNextRoundValidBeaconsCount(dst_as);

        NS_ASSERT(push_based_beacon_container.find(the_beacon.key) ==
                  push_based_beacon_container.end());
        push_based_beacon_container.insert(std::make_pair(the_beacon.key, the_beacon));
        Beacon* to_insert_beacon = &push_based_beacon_container.at(the_beacon.key);
        InsertToBeaconStoreImpl(dst_as, to_insert_beacon);

        InsertToAlgorithmDataStructures(to_insert_beacon,
                                        sender_as,
                                        remote_egress_if,
                                        local_ingress_if);
    }
}

void
BeaconServer::InsertToBeaconStoreImpl(uint16_t dst_as, Beacon* to_insert_beacon)
{
    uint16_t path_len = (uint16_t)to_insert_beacon->the_path.size();
    bool hasBeaconsForDstAS = beacon_store.find(dst_as) != beacon_store.end();

    if (hasBeaconsForDstAS)
    {
        if ([[maybe_unused]] auto hasBeaconWithRightLength =
                beacon_store.at(dst_as).find(path_len) != beacon_store.at(dst_as).end())
        {
            beacon_store.at(dst_as).at(path_len).insert(to_insert_beacon);
        }
        else
        {
            beacon_store.at(dst_as).insert(std::make_pair(path_len, beacons_with_equal_length()));
            beacon_store.at(dst_as).at(path_len).insert(to_insert_beacon);
        }
    }
    else
    {
        beacon_store.insert(std::make_pair(dst_as, beacons_with_same_dst_as()));
        beacon_store.at(dst_as).insert(std::make_pair(path_len, beacons_with_equal_length()));
        beacon_store.at(dst_as).at(path_len).insert(to_insert_beacon);
    }
}

void
BeaconServer::IncrementValidBeaconsCount(uint16_t dst_as)
{
    if (valid_beacons_count_per_dst_as.find(dst_as) == valid_beacons_count_per_dst_as.end())
    {
        valid_beacons_count_per_dst_as.insert(std::make_pair(dst_as, 1));
    }
    else
    {
        valid_beacons_count_per_dst_as.at(dst_as)++;
    }
}

void
BeaconServer::IncrementNextRoundValidBeaconsCount(uint16_t dst_as)
{
    if (next_round_valid_beacons_count_per_dst_as.find(dst_as) ==
        next_round_valid_beacons_count_per_dst_as.end())
    {
        next_round_valid_beacons_count_per_dst_as.insert(std::make_pair(dst_as, 1));
    }
    else
    {
        next_round_valid_beacons_count_per_dst_as.at(dst_as)++;
    }
}

void
BeaconServer::DeleteBeacon(Beacon* to_be_removed_beacon, ld replacement_key, uint16_t dst_as)
{
    if (to_be_removed_beacon->beacon_direction == BeaconDirectionT::PUSH_BASED)
    {
        NS_ASSERT(beacon_store.find(dst_as) != beacon_store.end());
        NS_ASSERT(beacon_store.at(dst_as).find(to_be_removed_beacon->the_path.size()) !=
                  beacon_store.at(dst_as).end());
        NS_ASSERT(beacon_store.at(dst_as)
                      .at(to_be_removed_beacon->the_path.size())
                      .find(to_be_removed_beacon) !=
                  beacon_store.at(dst_as).at(to_be_removed_beacon->the_path.size()).end());

        beacon_store.at(dst_as)
            .at(to_be_removed_beacon->the_path.size())
            .erase(to_be_removed_beacon);
        if (beacon_store.at(dst_as).at(to_be_removed_beacon->the_path.size()).empty())
        {
            beacon_store.at(dst_as).erase(to_be_removed_beacon->the_path.size());
        }
        if (beacon_store.at(dst_as).empty())
        {
            beacon_store.erase(dst_as);
        }
    }

    if (to_be_removed_beacon->beacon_direction == BeaconDirectionT::PUSH_BASED ||
        (to_be_removed_beacon->beacon_direction == BeaconDirectionT::PULL_BASED &&
         ORIGINATOR_PTR(to_be_removed_beacon) == GetAs()->AS()))
    {
        if (to_be_removed_beacon->is_new)
        {
            DecrementNextRoundValidBeaconsCount(dst_as);
        }
        else if (to_be_removed_beacon->is_valid)
        {
            DecrementValidBeaconsCount(dst_as);
            DecrementNextRoundValidBeaconsCount(dst_as);
        }
    }

    auto& beacon_container =
        to_be_removed_beacon->beacon_direction == BeaconDirectionT::PUSH_BASED
            ? push_based_beacon_container
            : ((ORIGINATOR_PTR(to_be_removed_beacon) == GetAs()->AS())
                   ? requested_pull_based_beacon_container
                   : non_requested_pull_based_beacon_container.at(pull_based_write));

    NS_ASSERT(beacon_container.find(to_be_removed_beacon->key) != beacon_container.end());

    DeleteFromAlgorithmDataStructures(to_be_removed_beacon, replacement_key);
    beacon_container.erase(to_be_removed_beacon->key);
}

void
BeaconServer::DecrementValidBeaconsCount(uint16_t dst_as)
{
    NS_ASSERT(valid_beacons_count_per_dst_as.find(dst_as) != valid_beacons_count_per_dst_as.end() &&
              valid_beacons_count_per_dst_as.at(dst_as) > 0);
    valid_beacons_count_per_dst_as.at(dst_as)--;
    if (valid_beacons_count_per_dst_as.at(dst_as) == 0)
    {
        valid_beacons_count_per_dst_as.erase(dst_as);
    }
}

void
BeaconServer::DecrementNextRoundValidBeaconsCount(uint16_t dst_as)
{
    NS_ASSERT(next_round_valid_beacons_count_per_dst_as.find(dst_as) !=
                  next_round_valid_beacons_count_per_dst_as.end() &&
              next_round_valid_beacons_count_per_dst_as.at(dst_as) > 0);
    next_round_valid_beacons_count_per_dst_as.at(dst_as)--;
    if (next_round_valid_beacons_count_per_dst_as.at(dst_as) == 0)
    {
        next_round_valid_beacons_count_per_dst_as.erase(dst_as);
    }
}

void
BeaconServer::IncrementControlPlaneBytesSent(Beacon& the_beacon, uint16_t interface)
{
    beacons_sent_per_interface.at(interface)++;
    beacons_sent_per_interface_per_period.at(now).at(interface)++;
    bytes_sent_per_interface_per_period.at(now).at(interface) +=
        (BEACON_HEADER_SIZE + BEACON_HOP_SIZE * the_beacon.the_path.size());

    auto dst_as = DST_AS(the_beacon);

    auto& counters_per_dst = beacons_sent_per_dst_per_interface.at(interface);
    if (counters_per_dst.find(dst_as) == counters_per_dst.end())
    {
        counters_per_dst.insert(std::make_pair(dst_as, 0));
    }
    counters_per_dst.at(dst_as)++;

    if (the_beacon.optimization_target != NULL)
    {
        auto& counters_per_opt = the_beacon.beacon_direction == BeaconDirectionT::PUSH_BASED
                                     ? push_based_beacons_sent_per_opt_per_interface.at(interface)
                                     : pull_based_beacons_sent_per_opt_per_interface.at(interface);
        if (counters_per_opt.find(the_beacon.optimization_target) == counters_per_opt.end())
        {
            counters_per_opt.insert(std::make_pair(the_beacon.optimization_target, 0));
        }
        counters_per_opt.at(the_beacon.optimization_target)++;
    }
}

/*!
  \param sender_as  The AS that forwarded the beacon to us ( in GenerateBeaconAndSend() )
  \param remote_if  The sender_as's egress_interface, on which it send the beacon towards us
  \param local_if   Our local ingress_if on which we received the beacon from sender_as
*/
void
BeaconServer::ReceiveBeacon(Beacon& received_beacon,
                            uint16_t sender_as,
                            uint16_t remote_if,
                            uint16_t local_if)
{
    uint16_t dst_as = DST_AS(received_beacon);

    NS_ASSERT(received_beacon.beacon_direction == BeaconDirectionT::PULL_BASED ||
              received_beacon.optimization_target == NULL ||
              ORIGINATOR(received_beacon) == received_beacon.optimization_target->target_as);
    bool to_import;
    bool path_exists;
    bool existing_path_valid;
    Beacon* beacon_to_replace;
    ld replacement_key;

    std::tie(to_import, path_exists, existing_path_valid, beacon_to_replace, replacement_key) =
        ImportPolicy(received_beacon, sender_as, remote_if, local_if, now);

    NS_ASSERT(received_beacon.beacon_direction != BeaconDirectionT::PUSH_BASED ||
              next_round_valid_beacons_count_per_dst_as.find(dst_as) !=
                  next_round_valid_beacons_count_per_dst_as.end() ||
              to_import);

    if (!to_import)
    {
        return;
    }

    if (!path_exists && beacon_to_replace != NULL)
    {
        NS_ASSERT(DST_AS_PTR(beacon_to_replace) == DST_AS(received_beacon));
        DeleteBeacon(beacon_to_replace, replacement_key, dst_as);
    }

    InsertBeacon(received_beacon,
                 dst_as,
                 sender_as,
                 remote_if,
                 local_if,
                 path_exists,
                 existing_path_valid,
                 beacon_to_replace);
}

std::tuple<bool, bool, bool, Beacon*, ld>
BeaconServer::ImportPolicy(Beacon& the_beacon,
                           uint16_t sender_as,
                           uint16_t remote_egress_if_no,
                           uint16_t self_ingress_if_no,
                           uint16_t now)
{
    if (the_beacon.beacon_direction == BeaconDirectionT::PUSH_BASED)
    {
        if (push_based_beacon_container.find(the_beacon.key) != push_based_beacon_container.end())
        {
            Beacon* existing_beacon = &push_based_beacon_container.at(the_beacon.key);
            NS_ASSERT(existing_beacon->beacon_direction == BeaconDirectionT::PUSH_BASED);
            NS_ASSERT(ORIGINATOR_PTR(existing_beacon) == ORIGINATOR(the_beacon));
            if (!existing_beacon->is_valid)
            {
                return std::tuple<bool, bool, bool, Beacon*, ld>(true,
                                                                 true,
                                                                 false,
                                                                 existing_beacon,
                                                                 0);
            }
            return std::tuple<bool, bool, bool, Beacon*, ld>(true, true, true, existing_beacon, 0);
        }

        if (the_beacon.the_path.size() == 1)
        {
            return std::tuple<bool, bool, bool, Beacon*, ld>(true, false, false, NULL, 0);
        }
    }

    return AlgSpecificImportPolicy(the_beacon,
                                   sender_as,
                                   remote_egress_if_no,
                                   self_ingress_if_no,
                                   now);
}

void
BeaconServer::UpdateStateBeforeBeaconing()
{
    now = (uint16_t)Simulator::Now().ToInteger(Time::MIN);
    next_period = now + (uint16_t)beaconing_period.ToInteger(Time::MIN);
    bytes_sent_per_interface_per_period.insert(
        std::make_pair(now, std::vector<uint32_t>(GetAs()->GetNInterfaces(), 0)));
    beacons_sent_per_interface_per_period.insert(
        std::make_pair(now, std::vector<uint32_t>(GetAs()->GetNInterfaces(), 0)));
}

const uint16_t
BeaconServer::GetCurrentTime() const
{
    return now;
}

void
BeaconServer::RegisterToLocalPathServer()
{
    std::map<int32_t, std::unordered_map<std::string, Beacon>&> beacon_containers = {
        {1, push_based_beacon_container},
        {2, requested_pull_based_beacon_container}};

    for (const auto& beacon_container : beacon_containers)
    {
        for (const auto& [key, the_beacon] : beacon_container.second)
        {
            if (the_beacon.is_new)
            {
                PathSegment path_segment;
                if (the_beacon.beacon_direction == BeaconDirectionT::PUSH_BASED)
                {
                    the_beacon.ExtractPathSegmentFromPushBasedBeacon(path_segment);
                }
                else
                {
                    the_beacon.ExtractPathSegmentFromPullBasedBeacon(path_segment);
                }

                if (dynamic_cast<ScionCoreAs*>(GetAs()) != NULL)
                {
                    GetAs()->GetPathServer()->RegisterCorePathSegment(path_segment, key);
                }
                else
                {
                    GetAs()->GetPathServer()->RegisterUpPathSegment(path_segment, key);
                }
            }
        }
    }
}

std::pair<ld, ld>
BeaconServer::CalculateFinalDiversityScores(Beacon* the_beacon)
{
    ld as_level_diversity_score = 0;
    ld link_level_diversity_score = 0;
    int32_t counter = 0;

    uint16_t dst_as = DST_AS_PTR(the_beacon);
    const auto& equal_dst_as_beacons = beacon_store.at(dst_as);
    for (const auto& len_beacons_pair : equal_dst_as_beacons)
    {
        const auto& beacons = len_beacons_pair.second;
        for (const auto& curr_beacon : beacons)
        {
            if (curr_beacon != the_beacon)
            {
                as_level_diversity_score +=
                    AsLevelJaccardDistanceBetweenTwoPaths(the_beacon, curr_beacon);
                link_level_diversity_score +=
                    LinkLevelJaccardDistanceBetweenTwoPaths(the_beacon, curr_beacon);
                counter++;
            }
        }
    }
    return (
        std::make_pair(as_level_diversity_score / counter, link_level_diversity_score / counter));
}

const std::vector<std::vector<ld>>&
BeaconServer::GetIntraAsEnergies() const
{
    return intra_as_energies;
}

float
BeaconServer::GetDirtyEnergyRatio() const
{
    return dirty_energy_ratio;
}

float
BeaconServer::GetSunEnergyRatio() const
{
    return sun_energy_ratio;
}

const std::unordered_map<uint16_t, beacons_with_same_dst_as>&
BeaconServer::GetBeaconStore() const
{
    return beacon_store;
}

const std::unordered_map<std::string, Beacon>&
BeaconServer::GetPathMapToBeacon() const
{
    return push_based_beacon_container;
}

const std::unordered_map<uint16_t, uint32_t>&
BeaconServer::GetValidBeaconsCountPerDstAs() const
{
    return valid_beacons_count_per_dst_as;
}

const std::unordered_map<uint16_t, uint32_t>&
BeaconServer::GetNextRoundValidBeaconsCountPerDstAs() const
{
    return next_round_valid_beacons_count_per_dst_as;
}

const std::unordered_map<uint16_t, std::vector<uint32_t>>&
BeaconServer::GetBytesSentPerInterfacePerPeriod() const
{
    return bytes_sent_per_interface_per_period;
}

const std::vector<std::unordered_map<uint16_t, uint32_t>>&
BeaconServer::GetBeaconsSentPerDstPerInterfacePerPeriod() const
{
    return beacons_sent_per_dst_per_interface;
}

const std::vector<std::unordered_map<const OptimizationTarget*, uint32_t>>&
BeaconServer::GetPushBasedBeaconsSentPerOptPerInterfacePerPeriod() const
{
    return push_based_beacons_sent_per_opt_per_interface;
}

const std::vector<std::unordered_map<const OptimizationTarget*, uint32_t>>&
BeaconServer::GetPullBasedBeaconsSentPerOptPerInterfacePerPeriod() const
{
    return pull_based_beacons_sent_per_opt_per_interface;
}

const std::unordered_map<uint16_t, std::vector<uint32_t>>&
BeaconServer::GetBeaconsSentPerInterfacePerPeriod() const
{
    return beacons_sent_per_interface_per_period;
}

const std::vector<uint64_t>&
BeaconServer::GetBeaconsSentPerInterface() const
{
    return beacons_sent_per_interface;
}

void
BeaconServer::ReadBeacons()
{
    if (file_to_read_beacons == "none")
    {
        return;
    }

    nlohmann::json beacons_json;
    std::ifstream file(file_to_read_beacons);
    file >> beacons_json;
    file.close();

    for (const auto& beacon_json : beacons_json)
    {
        static_info_extension_t static_info_extension;
        path the_path = beacon_json["path"].get<std::vector<uint64_t>>();
        isd_path the_isd_path = beacon_json["isd_path"].get<std::vector<uint16_t>>();
        auto key_v = beacon_json["key"].get<std::vector<std::string>>();
        std::string key = StrJoin(key_v,""); //std::string(key_v.begin(), key_v.end());
        push_based_beacon_container.insert(std::make_pair(key,
                                                          Beacon(static_info_extension,
                                                                 NULL,
                                                                 BeaconDirectionT::PUSH_BASED,
                                                                 beacon_json["initiation_time"],
                                                                 beacon_json["expiration_time"],
                                                                 beacon_json["initiation_time"],
                                                                 beacon_json["expiration_time"],
                                                                 false,
                                                                 true,
                                                                 the_path,
                                                                 key,
                                                                 the_isd_path)));

        Beacon* to_insert_beacon = &push_based_beacon_container.at(key);
        uint16_t path_len = (uint16_t)to_insert_beacon->the_path.size();
        uint16_t dst_as = DST_AS_PTR(to_insert_beacon);

        if (beacon_store.find(dst_as) != beacon_store.end() &&
            beacon_store.at(dst_as).find(path_len) != beacon_store.at(dst_as).end())
        {
            beacon_store.at(dst_as).at(path_len).insert(to_insert_beacon);
        }
        else if (beacon_store.find(dst_as) != beacon_store.end() &&
                 beacon_store.at(dst_as).find(path_len) == beacon_store.at(dst_as).end())
        {
            beacon_store.at(dst_as).insert(std::make_pair(path_len, beacons_with_equal_length()));
            beacon_store.at(dst_as).at(path_len).insert(to_insert_beacon);
        }
        else
        {
            beacon_store.insert(std::make_pair(dst_as, beacons_with_same_dst_as()));
            beacon_store.at(dst_as).insert(std::make_pair(path_len, beacons_with_equal_length()));
            beacon_store.at(dst_as).at(path_len).insert(to_insert_beacon);
        }

        IncrementValidBeaconsCount(dst_as);
        IncrementNextRoundValidBeaconsCount(dst_as);
    }
}

void
BeaconServer::WriteBeacons()
{
    if (file_to_write_beacons == "none")
    {
        return;
    }

    nlohmann::json beacons_json;
    for (const auto& [key, the_beacon] : push_based_beacon_container)
    {
        nlohmann::json beacon_json;

       // std::vector<uint16_t> v(key.begin(), key.end());
       auto v = Split( key, BEACON_KEY_WIDTH );
        beacon_json["key"] = nlohmann::json(v);
        beacon_json["initiation_time"] = 0;
        beacon_json["expiration_time"] = 0xFFFF;
        beacon_json["direction"] = "push";
        beacon_json["path"] = nlohmann::json(the_beacon.the_path);
        beacon_json["isd_path"] = nlohmann::json(the_beacon.the_isd_path);
        beacons_json.push_back(beacon_json);
    }

    std::ofstream file(file_to_write_beacons);
    file << beacons_json.dump();
    file.close();
}

} // namespace ns3

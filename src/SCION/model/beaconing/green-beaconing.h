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

#ifndef SCION_SIMULATOR_GREEN_BEACONING_H
#define SCION_SIMULATOR_GREEN_BEACONING_H

#include <cmath>
#include <yaml-cpp/yaml.h>

#include "beacon-server.h"
#include "ns3/utils.h"

namespace ns3 {

#define MAX_BEACONS_TO_SEND_PER_IFACE 1
#define MAX_BEACONS_TO_STORE_PER_IFACE 1

class GreenBeaconing : public BeaconServer
{
public:
  GreenBeaconing (ScionAs *as, bool parallel_scheduler, rapidxml::xml_node<> *xml_node,
                  const YAML::Node &config)
      : BeaconServer (as, parallel_scheduler, xml_node, config)
  {
  }

  void DoInitializations (uint32_t num_ases, rapidxml::xml_node<> *xml_node,
                          const YAML::Node &config) override;

  const std::vector<std::vector<std::multimap<ld, Beacon *>>> &GetBeaconsSortedByPollution () const;

private:
  std::vector<std::vector<std::multimap<ld, Beacon *>>>
      beacons_per_dst_per_ing_if_sorted_by_pollution;

  void DisseminateBeacons (NeighbourRelation relation) override;

  std::tuple<bool, bool, bool, Beacon *, ld>
  AlgSpecificImportPolicy (Beacon &the_beacon, uint16_t sender_as, uint16_t remote_egress_if_no,
                              uint16_t self_ingress_if_no, uint16_t now) override;

  void InsertToAlgorithmDataStructures (Beacon *the_beacon, uint16_t sender_as,
                                            uint16_t remote_egress_if_no,
                                            uint16_t self_ingress_if_no) override;

  void DeleteFromAlgorithmDataStructures (Beacon *the_beacon, ld replacement_key) override;

  void CreateInitialStaticInfoExtension (static_info_extension_t &static_info_extension,
                                        uint16_t self_egress_if_no,
                                        const OptimizationTarget *optimization_target) override;

  void UpdateAlgorithmDataStructuresPeriodic (Beacon *the_beacon, bool invalidated) override;

  void SelectBeaconsToDisseminatePerDstPerNbr (
      uint16_t remote_as_no, uint16_t dst_as_no,
      const beacons_with_same_dst_as &beacons_to_the_dst_as,
      std::multimap<ld,
                    std::tuple<Beacon *, uint16_t, uint16_t, ScionAs *, static_info_extension_t>>
          &pollution_index_map_to_beacon_and_metadata);

  ld CalculatePollutionBetweenBorderRouters (uint16_t ingress_if, uint16_t egress_if);
};

} // namespace ns3
#endif //SCION_SIMULATOR_GREEN_BEACONING_H

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

#ifndef SCION_SIMULATOR_PRE_SIMULATION_SETUP_H
#define SCION_SIMULATOR_PRE_SIMULATION_SETUP_H

#include "beaconing/baseline.h"
#include "beaconing/beacon-server.h"
#include "beaconing/diversity-age-based.h"
#include "beaconing/green-beaconing.h"
#include "beaconing/latency-optimized-beaconing.h"
#include "beaconing/scionlab-algo.h"

#include "path-server.h"
// #include "post-simulation-evaluations.h"

#include "scion-as.h"
#include "scion-core-as.h"
#include "scion-host.h"
#include "time-server.h"
// #include "user-defined-events.h"
// #include "utils.h"

/*
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/nstime.h"
#include "ns3/point-to-point-channel.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/ptr.h"

#include <istream>
#include <omp.h>
#include <random>
#include <set>*/
#include <yaml-cpp/yaml.h>
#include <string>

namespace ns3
{
void SetTimeResolution(const std::string& time_res_str);

// rapidxml::xml_node<>* SetupTopologyFile (std::string topology_name);

void
InstallBorderRouters(ScionAs* from_as,
                     ScionAs* to_as,
                     bool only_propagation_delay,
                     double latitude,
                     double longitude);

// void InstantiatePathServers(const YAML::Node& config, const ns3::NodeContainer& as_nodes);





bool OnlyPropagationDelay(const YAML::Node& config);
} // namespace ns3
#endif // SCION_SIMULATOR_PRE_SIMULATION_SETUP_H

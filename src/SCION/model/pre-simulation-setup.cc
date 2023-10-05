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

#include "pre-simulation-setup.h"
#include "ns3/scion-simulation-context.h"

namespace ns3
{
void
SetTimeResolution(const std::string& time_res_str)
{
    if (time_res_str == "FS")
    {
        Time::SetResolution(Time::FS);
    }
    else if (time_res_str == "PS")
    {
        Time::SetResolution(Time::PS);
    }
    else if (time_res_str == "NS")
    {
        Time::SetResolution(Time::NS);
    }
    else if (time_res_str == "US")
    {
        Time::SetResolution(Time::US);
    }
    else if (time_res_str == "MS")
    {
        Time::SetResolution(Time::MS);
    }
    else if (time_res_str == "S")
    {
        Time::SetResolution(Time::S);
    }
    else if (time_res_str == "MIN")
    {
        Time::SetResolution(Time::MIN);
    }
}




/*!
    \details add one BorderRouter to both ScionAS, each in the same location (lat,long)
            where both ASes have a common point of presence
*/
void
InstallBorderRouters(ScionAs* from_as,
                     ScionAs* to_as,
                     bool only_propagation_delay,
                     double latitude,
                     double longitude)
{
    Time to_propagation_delay, from_propagation_delay;
    Time to_transmission_delay, from_transmission_delay;
    Time to_processing_delay, from_processing_delay;
    Time to_processing_throughput_delay, from_processing_throughput_delay;

    to_propagation_delay =
        NanoSeconds(5); // Assuming 1m fiber optic between neighboring devices in the same location
    from_propagation_delay = NanoSeconds(5);

    if (only_propagation_delay)
    {
        to_transmission_delay = Time(0);
        from_transmission_delay = Time(0);

        to_processing_delay = Time(0);
        from_processing_delay = Time(0);

        to_processing_throughput_delay = Time(0);
        from_processing_throughput_delay = Time(0);
    }
    else
    {
        to_transmission_delay =
            PicoSeconds(20); // Per byte transmission delay assuming 400 Gbps link
        from_transmission_delay = PicoSeconds(20);

        to_processing_delay = NanoSeconds(10);
        from_processing_delay = NanoSeconds(10);

        to_processing_throughput_delay = PicoSeconds(200); // 5 Giga packets per second
        from_processing_throughput_delay = PicoSeconds(200);
    }

    BorderRouter* to_br =
        to_as->AddBr(latitude, longitude, to_processing_delay, to_processing_throughput_delay);
        
    BorderRouter* from_br = from_as->AddBr(latitude,
                                           longitude,
                                           from_processing_delay,
                                           from_processing_throughput_delay);

    to_br->AddToPropagationDelays(to_propagation_delay);
    to_br->AddToTransmissionDelays(to_transmission_delay);

    from_br->AddToPropagationDelays(from_propagation_delay);
    from_br->AddToTransmissionDelays(from_transmission_delay);

    // dont get confused, no one installs NetDevices on BorderRouters
    // no BorderRouters ScionCapableNode:: GetNInterfaces() gets called not Node::GetNDevices
    // it simply returns the Number of  propagation-delays added so far
    auto from_br_if = from_br->GetNDevices() - 1;
    auto to_br_if = to_br->GetNDevices() -1;

    to_br->AddToIfForwadingTable(to_as->GetNInterfaces() - 1, to_br_if );
    from_br->AddToIfForwadingTable(from_as->GetNInterfaces() - 1, from_br_if );

    to_br->AddToRemoteNodesInfo(from_br,
                                from_br_if,
                                from_as->ISD(),
                                from_as->AS());
    from_br->AddToRemoteNodesInfo(to_br,
                                  to_br_if,
                                  to_as->ISD(),
                                  to_as->AS());
}


bool
OnlyPropagationDelay(const YAML::Node& config)
{
    if (config["only_propagation_delay"] && config["only_propagation_delay"].as<int32_t>() != 0)
    {
        return true;
    }

    return false;
}
} // namespace ns3
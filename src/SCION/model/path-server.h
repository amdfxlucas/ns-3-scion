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

#ifndef SCION_SIMULATOR_PATH_SERVER_H
#define SCION_SIMULATOR_PATH_SERVER_H

#include "path-segment.h"
#include "scion-capable-node.h"
#include "scion-packet.h"

#include "ns3/nstime.h"

#include <map>
#include <unordered_map>
#include <vector>

namespace ns3
{
class PathServer : public ScionCapableNode
{
  public:
    PathServer(uint32_t system_id,
               uint16_t isd_number,
               uint16_t as_number,
               host_addr_t local_address,
               double latitude,
               double longitude,
               ScionAs* as)
        : ScionCapableNode(system_id, isd_number, as_number, local_address, latitude, longitude, as)
    {
        set_of_all_core_ases.insert(ia_addr);
    }

    void RegisterCorePathSegment(PathSegment& path_segment, std::string key);
    void RegisterUpPathSegment(PathSegment& path_segment, std::string key);
    void RegisterDownPathSegment(PathSegment& path_segment, std::string key);

  private:
    std::set<ia_t> set_of_all_core_ases;

    registered_path_segs_dataset_t registered_core_segments;
    registered_path_segs_dataset_t registered_up_segments;
    registered_path_segs_dataset_t registered_down_segments;

    cached_path_segs_dataset_t cached_core_segments;
    cached_path_segs_dataset_t cached_up_segments;
    cached_path_segs_dataset_t cached_down_segments;

    void ProcessReceivedPacket(uint16_t local_if, ScionPacket* packet, Time receive_time) override;

    void ProcessLocalHostRequestForPath(PathSegmentType path_type,
                                        ia_t src_ia,
                                        ia_t dst_ia,
                                        host_addr_t host_addr);

    void SendRegisteredPathToLocalHost(host_addr_t host_addr,
                                       PathSegmentType path_type,
                                       ia_t src_ia,
                                       ia_t dst_ia,
                                       const reg_path_segs_to_one_as_t*);

    void ReturnListOfAllCoreAses(host_addr_t host_addr);
};
} // namespace ns3

#endif // SCION_SIMULATOR_PATH_SERVER_H

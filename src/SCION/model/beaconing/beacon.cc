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

#include "beacon.h"

#include "ns3/externs.h"
#include "ns3/path-segment.h"
#include "ns3/utils.h"

namespace ns3
{
/*!

Beacon-Path: (from originator left to destination right )

link-info0          |         link-info1        |      link-info2
                    |                           |
send_as0 rec_as0    |      send_as1 rec_as1     |       send_as2 rec_as2
eg_if0   ing_if0    |      eg_if1   ing_if1     |       eg_if2   ing_if2
                    |                           |
According Path-Segment: originator is send_as0

              hop2(last hop)  |      hop(normal hop)             |                                 |
16      16       16      16   | 16    16       16         16     |  16    16        16      16     |16    16       16      16 
ISD     AS      in_if   eg_if | ISD    AS      in_if    eg_if    | ISD    AS      in_if   eg_if    | ISD   AS      in_if  eg_if 
isd2  rec_as2  ing_if2  ---   | isd2  send_as2  in_if1   eg_if2  | isd1  send_as1  in_if0   eg_if1 |isd0  send_as0  ---  eg_if0



*/
void
Beacon::ExtractPathSegmentFromPushBasedBeacon(PathSegment& path_segment) const
{
    path_segment.initiation_time = next_initiation_time;
    path_segment.expiration_time = next_expiration_time;

    path_segment.originator =
        (((uint32_t)the_isd_path.at(0)) << 16) | (((uint32_t)UPPER_16_BITS(the_path.at(0))));

    uint64_t previous_hop = 0;
    bool last_hop = true;
    for (std::vector<uint64_t>::const_reverse_iterator hop = the_path.rbegin();
         hop != the_path.rend();
         ++hop)
    {
        uint16_t ingress = 0;
        uint16_t egress = 0;
        uint16_t as = 0;

        if (last_hop)
        {
            as = SECOND_LOWER_16_BITS(*hop); // corresponds to the receiver_as of the_path.back()
            ingress = LOWER_16_BITS(*hop);
            last_hop = false;
        }
        else
        {
            as = UPPER_16_BITS(previous_hop);            // sender_as of previous hop
            egress = SECOND_UPPER_16_BITS(previous_hop); // egress of previous hop
            ingress = LOWER_16_BITS(*hop);
        }

        uint16_t isd = as_to_isd_map.at(as);
        previous_hop = *hop;
        uint64_t hop_field = (((uint64_t)isd) << 48) | (((uint64_t)as) << 32) |
                             (((uint64_t)ingress) << 16) | ((uint64_t)egress);
        path_segment.hops.push_back(hop_field);
    }

    uint16_t egress = SECOND_UPPER_16_BITS(previous_hop); // previos_hop now points to path.front()
    uint16_t ingress = 0;
    uint16_t as = UPPER_16_BITS(previous_hop); // sender_as of prev_hop
    uint16_t isd = as_to_isd_map.at(as);

    uint64_t hop_field = (((uint64_t)isd) << 48) | (((uint64_t)as) << 32) |
                         (((uint64_t)ingress) << 16) | ((uint64_t)egress);
    path_segment.hops.push_back(hop_field);
}

/*!

Beacon-Path: (from destination left to originator right )

link-info0          |         link-info1        |      link-info2
                    |                           |
send_as0 rec_as0    |      send_as1 rec_as1     |       send_as2 rec_as2
eg_if0   ing_if0    |      eg_if1   ing_if1     |       eg_if2   ing_if2
                    |                           |
According Path-Segment: originator is rec_as2

              hop2(last hop)  |      hop(normal hop)             |                                 |
16      16       16      16   | 16    16       16         16     |  16    16        16      16     |16    16         16    16 
ISD     AS      in_if   eg_if | ISD    AS      in_if    eg_if    | ISD    AS       in_if   eg_if    | ISD   AS     in_if  eg_if 
isd0  send_as0 eg_if0  ---    | isd0  rec_as0  eg_if1   in_if0  | isd1  rec_as1  eg_if2   in_if1   | isd2  rec_as2 ---    in_if2



*/
void
Beacon::ExtractPathSegmentFromPullBasedBeacon(PathSegment& path_segment) const
{
    path_segment.initiation_time = next_initiation_time;
    path_segment.expiration_time = next_expiration_time;

    path_segment.originator = (((uint32_t)the_isd_path.back()) << 16) |
                              (((uint32_t)SECOND_LOWER_16_BITS(the_path.back())));

    uint64_t previous_hop = 0;
    bool last_hop = true;
    for (std::vector<uint64_t>::const_iterator hop = the_path.begin(); hop != the_path.end(); ++hop)
    {
        uint16_t ingress = 0;
        uint16_t egress = 0;
        uint16_t as = 0;

        if (last_hop)
        {
            as = UPPER_16_BITS(*hop);
            ingress = SECOND_UPPER_16_BITS(*hop);
            last_hop = false;
        }
        else
        {
            as = SECOND_LOWER_16_BITS(previous_hop);
            egress = LOWER_16_BITS(previous_hop);
            ingress = SECOND_UPPER_16_BITS(*hop);
        }

        uint16_t isd = as_to_isd_map.at(as);
        previous_hop = *hop;
        uint64_t hop_field = (((uint64_t)isd) << 48) | (((uint64_t)as) << 32) |
                             (((uint64_t)ingress) << 16) | ((uint64_t)egress);
        path_segment.hops.push_back(hop_field);
    }

    uint16_t egress = LOWER_16_BITS(previous_hop);
    uint16_t ingress = 0;
    uint16_t as = SECOND_LOWER_16_BITS(previous_hop);
    uint16_t isd = as_to_isd_map.at(as);

    uint64_t hop_field = (((uint64_t)isd) << 48) | (((uint64_t)as) << 32) |
                         (((uint64_t)ingress) << 16) | ((uint64_t)egress);
    path_segment.hops.push_back(hop_field);
}
} // namespace ns3
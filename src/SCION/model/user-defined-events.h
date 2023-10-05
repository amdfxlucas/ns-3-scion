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

#ifndef SCION_SIMULATOR_USER_DEFINED_EVENTS_H
#define SCION_SIMULATOR_USER_DEFINED_EVENTS_H

#include "json.hpp"
#include "path-segment.h"
#include "scion-as.h"
#include "scion-packet.h"

#include "ns3/simulator.h"
#include <yaml-cpp/yaml.h>

#include "ns3/scion-simulation-context.h"

#include <any>
#include <fstream>
#include <functional>
#include <unordered_map>



template <typename Ret>
struct AnyCallable
{
    AnyCallable()
    {
    }

    template <typename... Args>
    AnyCallable(std::function<Ret(Args...)> fun)
        : m_any(fun)
    {
    }

    template <typename... Args>
    Ret operator()(Args... args)
    {
        return std::invoke(std::any_cast<std::function<Ret(Args...)>>(m_any),
                           std::forward<Args>(args)...);
    }

    template <std::size_t... S, typename T>
    Ret operator()(const std::vector<T>& vec, std::index_sequence<S...>)
    {
        return operator()(vec[S]...);
    }

    template <std::size_t size, typename T>
    Ret operator()(const std::vector<T>& vec)
    {
        return operator()(vec, std::make_index_sequence<size>());
    }

    std::any m_any;
};


namespace ns3
{

class UserDefinedEvents
{
  public:
    UserDefinedEvents( SCIONSimulationContext& ctx                     )
        : m_ctx(ctx)          
    {
        if (!m_ctx.config["events_file"])
        {
            this->~UserDefinedEvents();
            return;
        }

        ConstructFuncMap();
        ReadAndScheduleUserDefinedEvents( m_ctx.config["events_file"].as<std::string>());
    }

  private:
  SCIONSimulationContext& m_ctx;
 
    std::unordered_map<std::string, AnyCallable<void>> function_name_to_function;

    void ConstructFuncMap();

    void ReadAndScheduleUserDefinedEvents(const std::string& events_file_str);

    void RunUserSpecifiedEvent(const std::string& func_name, std::vector<std::string> vec);

    void AddAHost(std::string isd_number, std::string real_as_no, std::string local_address);

    void LinkDown(std::string isd_number, std::string real_as_no, std::string if_id);
    void LinkUp(std::string isd_number, std::string real_as_no, std::string if_id);

    void SendAPacket(std::string src_isd_number,
                     std::string real_src_as_no,
                     std::string src_local_address,
                     std::string dst_isd_number,
                     std::string real_dst_as_no,
                     std::string dst_local_address,
                     std::string pyload_size);

    void SendPacketBatch(std::string src_isd_number,
                         std::string real_src_as_no,
                         std::string src_local_address,
                         std::string dst_isd_number,
                         std::string real_dst_as_no,
                         std::string dst_local_address,
                         std::string pyload_size,
                         std::string no_pkts);

    void TimeReferencesDown();
    void TimeReferencesUp();
};
} // namespace ns3
#endif // SCION_SIMULATOR_USER_DEFINED_EVENTS_H

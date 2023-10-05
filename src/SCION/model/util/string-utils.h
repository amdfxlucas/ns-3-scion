#pragma once
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
namespace ns3
{

std::vector<std::string> Split(const std::string& str, int splitLength);


// this can be replaced with std::format() if available
std::string toNdigit_string( auto number, int digitCount)
{
    std::stringstream ss;
            ss << std::setw( digitCount )<< std::setfill('0') << number;
            return ss.str();
}


/*
#include <fmt/format.h>
auto s = fmt::format("{}",fmt::join(elems,delim)); 
*/
std::string StrJoin(const std::vector<std::string> &lst, const std::string &delim);

// for string delimiter
std::vector<std::string> splitByDelim(std::string s, std::string delimiter);

}
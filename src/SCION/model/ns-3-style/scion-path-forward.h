#pragma once
#include <stdint.h>
namespace ns3
{
// Type indicates the type of the path contained in the SCION header.
//using pathType_t = uint8_t;

enum class pathType_t : uint8_t
{
    EmptyPath,
    RawPath,  // corresponds to scion.PathType ('1')
    OneHopPath,
    EpicPath, 
    DecodedPath

};

// PathType is uint8 so 256 values max.
#define MAXPATHTYPE  256
}
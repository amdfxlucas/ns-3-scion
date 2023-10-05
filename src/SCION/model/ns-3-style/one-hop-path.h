#pragma once
#include "ns3/hop-field.h"
#include "ns3/info-field.h"
#include "ns3/scion-path.h"
#include "ns3/buffer.h"

namespace ns3
{

    class DecodedPath;
    // Path encodes a one hop path. A one hop path is a special path that is created by a SCION router
// in the first AS and completed by a SCION router in the second AS. It is used during beaconing
// when there is not yet any other path segment available.
struct OneHopPath  
{
	InfoField _info  ;
	HopField _firstHop ;
	HopField _secondHop ;

     uint32_t Deserialize( Buffer::Iterator );
    void Serialize( Buffer::Iterator ) const;
   void Reverse();
    constexpr uint32_t Len() const{ return INFO_FIELD_LEN + 2*HOPLEN;};
   	constexpr pathType_t Type()const{ return pathType_t::OneHopPath ;};

    static Path ConvertTo(const OneHopPath& );
	static OneHopPath ConvertFrom(const Path& );

    static DecodedPath ConvertToDecoded( const OneHopPath& );
};




}
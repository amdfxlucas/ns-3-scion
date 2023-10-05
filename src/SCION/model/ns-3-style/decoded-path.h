#pragma once
#include "ns3/scion-path.h"

#include "ns3/base-field.h"
#include "ns3/hop-field.h"
#include "ns3/info-field.h"
#include "ns3/go-errors.h"
#include <expected>

namespace ns3 
{
	// MaxINFs is the maximum number of info fields in a SCION path.
#define	MaxINFs  3
	// MaxHops is the maximum number of hop fields in a SCION path.
#define	MaxHops 64

class RawPath;

// Decoded implements the SCION (data-plane) path type. Decoded is intended to be used in
// non-performance critical code paths, where the convenience of having a fully parsed path trumps
// the loss of performance.
struct DecodedPath
{
	Base base; // maybe inherit from Base

	// InfoFields contains all the InfoFields of the path.
	std::vector< InfoField > InfoFields;
	// HopFields contains all the HopFields of the path.
	std::vector<HopField> HopFields;

	bool IsXover()const { return base.IsXover(); }
	bool IsFirstHopAfterXover()const { return base.IsFirstHopAfterXover();}
	error IncPath(){ return base.IncPath(); }

	InfoField GetInfoField( int index) const;
InfoField GetCurrentInfoField()const;
HopField GetCurrentHopField() const;
HopField GetHopField(int index ) const;

auto operator<=>(const DecodedPath& other )const =default;

    // uint32_t Deserialize( Buffer::Iterator );
    // void Serialize( Buffer::Iterator ) const;

	std::expected<uint32_t,error> Deserialize( const_buffer_iterator );
    void Serialize( buffer_iterator ) const;


    uint16_t Len() const{ return base.Len() ;/*+ InfoFields.size()*INFO_FIELD_LEN + HopFields.size()*HOPLEN;*/	 };

   	constexpr static pathType_t Type() { return pathType_t::DecodedPath ;};
    auto GetCurrINF()const{return base.PathMeta.CurrINF;}
	//link_information_t GetCurrentHopField()const;

    // Reverse reverses a SCION path.
    DecodedPath& Reverse();

	RawPath ToRaw()const;

	static Path ConvertTo(const DecodedPath& );
	static DecodedPath ConvertFrom(const Path& );

	friend std::ostream& operator<<( std::ostream&, const DecodedPath& );
};

inline std::ostream&
operator<<(std::ostream& os, const DecodedPath& dp)
{
    os << dp.base;

    for (const auto& info : dp.InfoFields)
    {
        os << "info: " << info;
    }

    for (const auto& hop : dp.HopFields)
    {
        os << "hop: " << hop;
    }

    return os;
}

}// namespace ns3
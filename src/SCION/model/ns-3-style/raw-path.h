#pragma once
#include "ns3/buffer.h"
#include "ns3/base-field.h"
#include "ns3/info-field.h"
#include "ns3/scion-path.h"
#include "ns3/hop-field.h"
#include "ns3/decoded-path.h"

#include "ns3/iterator-util.h"
#include "neo/bytes.hpp"
#include "ns3/go-errors.h"
#include <expected>

namespace ns3
{


class DecodedPath;
class SCIONTestData;

// Raw is a raw representation of the SCION (data-plane) path type. 
//It is designed to parse as little as possible and should be used if performance matters.
class RawPath
{
	
friend class SCIONTestData;
friend class DecodedPath;
RawPath( const Base& b, neo::const_buffer buff );
RawPath( const Base& b, neo::bytes&& );
public:

RawPath()=default;

RawPath( neo::const_buffer buff );
RawPath( neo::bytes&&  );


RawPath( Base&& b):base(std::move(b)){} // used for tests


error IncPath();
bool IsLastHop()const;


// SetInfoField updates the InfoField at a given index.
void SetInfoField( const InfoField& info, int idx );
// SetHopField updates the HopField at a given index.
void SetHopField( const HopField& , int idx );

std::expected<InfoField,error> GetInfoField( int index) const;
std::expected<InfoField,error> GetCurrentInfoField()const;
std::expected<HopField,error> GetCurrentHopField() const;
std::expected<HopField,error> GetHopField(int index ) const;

bool IsFirstHop()const;
bool IsPenultimateHop()const;


	std::expected<uint32_t,error> Deserialize( const_buffer_iterator );
	void Serialize( buffer_iterator ) const;


  // Path& Reverse();
    void Reverse();

    uint32_t Len() const{ return base.Len();};
   	constexpr pathType_t Type()const{ return pathType_t::RawPath ;};

    // static Path ConvertTo(const RawPath& );
	// static RawPath ConvertFrom(const Path& );
	DecodedPath ConvertToDecoded()const;

	auto operator<=>( const RawPath& other ) const = default;
	operator std::string()const{ return static_cast<std::string>(base); }
	friend std::ostream& operator<< (std::ostream& out, const RawPath& raw);

	
private:

Base base;
neo::bytes raw;

};


inline std::ostream& operator<< (std::ostream& out, const RawPath& raw)
{
 return	out << static_cast<std::string>(raw) << std::endl;
}

}

/*

// Reverse reverses the path such that it can be used in the reverse direction.
func (s *Raw) Reverse() (path.Path, error) {
	// XXX(shitz): The current implementation is not the most performant, since it parses the entire
	// path first. If this becomes a performance bottleneck, the implementation should be changed to
	// work directly on the raw representation.

	decoded, err := s.ToDecoded()
	if err != nil {
		return nil, err
	}
	reversed, err := decoded.Reverse()
	if err != nil {
		return nil, err
	}
	if err := reversed.SerializeTo(s.Raw); err != nil {
		return nil, err
	}
	err = s.DecodeFromBytes(s.Raw)
	return s, err
}

// ToDecoded transforms a scion.Raw to a scion.Decoded.
func (s *Raw) ToDecoded() (*Decoded, error) {
	// Serialize PathMeta to ensure potential changes are reflected Raw.
	if err := s.PathMeta.SerializeTo(s.Raw[:MetaLen]); err != nil {
		return nil, err
	}
	decoded := &Decoded{}
	if err := decoded.DecodeFromBytes(s.Raw); err != nil {
		return nil, err
	}
	return decoded, nil
}













*/
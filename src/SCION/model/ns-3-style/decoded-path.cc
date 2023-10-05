#include "ns3/decoded-path.h"
#include "ns3/raw-path.h"
#include "neo/bytes.hpp"
#include "neo/bytewise_iterator.hpp"
#include "neo/as_buffer.hpp"
#include "ns3/basic-error.h"

namespace ns3
{

Path DecodedPath::ConvertTo(const DecodedPath& dp)
{
    DecodedPath tmp{dp};
    return Path( std::move(tmp) );
}

DecodedPath DecodedPath::ConvertFrom(const Path& path)
    {
        NS_ASSERT( path.Type() == pathType_t::DecodedPath
                || path.Type()== pathType_t::RawPath );
        DecodedPath dp;

       /* Buffer buff;
        path.Serialize(buff.Begin() );
        
        dp.Deserialize(buff.Begin());
        */
        neo::bytes buff{ path.Len() };

        path.Serialize( neo::bytewise_iterator( neo::as_buffer( buff ) ) );

        dp.Deserialize( neo::bytewise_iterator( neo::as_buffer(buff) ) );


        return dp;
    }

InfoField DecodedPath::GetInfoField( int index) const
{
 return InfoFields.at(index);
}

InfoField DecodedPath::GetCurrentInfoField()const
{
return GetInfoField( base.PathMeta.CurrINF );
}

HopField DecodedPath::GetCurrentHopField() const
{
 return GetHopField( base.PathMeta.CurrHF );
}

HopField DecodedPath::GetHopField(int index ) const
{
return HopFields.at(index);
}

// should the return type be Path& ?!
DecodedPath& DecodedPath::Reverse()
{	if ( base.NumINF == 0 )
    {
	//	return nil, serrors.New("empty decoded path is invalid and cannot be reversed")
	}

	// Reverse order of InfoFields and SegLens
//	for( int i = 0; ; i++ )
    {
        for( int j = base.NumINF-1, i=0 ; i < j; --j, ++i )
		{
            std::swap( InfoFields[i],InfoFields[j] );
            // InfoFields[i], InfoFields[j] = InfoFields[j], InfoFields[i]

            std::swap( base.PathMeta.SegLen[i], base.PathMeta.SegLen[j] );
		    // base.PathMeta.SegLen[i], PathMeta.SegLen[j] = PathMeta.SegLen[j], PathMeta.SegLen[i]
        }
   //     break;
	}
	// Reverse cons dir flags
	for ( int i = 0; i < base.NumINF; i++ )
    {
		auto& info = InfoFields[i];
		info.ConsDir = !info.ConsDir;
	}
	// Reverse order of hop fields
//	for ( int i = 0; ;++i )
    {
        for( int j= base.NumHops-1, i = 0; i < j; j--, ++i )
            std::swap( HopFields[i], HopFields[j]);
		// HopFields[i], s.HopFields[j] = s.HopFields[j], s.HopFields[i]
	}
	// Update CurrINF and CurrHF and SegLens
	base.PathMeta.CurrINF = uint8_t(base.NumINF) - base.PathMeta.CurrINF - 1;
	base.PathMeta.CurrHF = uint8_t(base.NumHops) - base.PathMeta.CurrHF - 1;

	return *this;
}

/*
// SerializeTo writes the path to a slice. The slice must be big enough to hold the entire data,
// otherwise an error is returned.
void
DecodedPath::Serialize(Buffer::Iterator start) const
{
    base.PathMeta.Serialize(start);
    start+=base.PathMeta.GetSerializedSize();

    for (auto& info : InfoFields)
    {
        info.Serialize(start);
        start+= INFO_FIELD_LEN;
    }
    for (auto& hop : HopFields)
    {
        hop.Serialize(start);
        start+=HOPLEN;
    }
}

// DecodeFromBytes fully decodes the SCION path into the corresponding fields.
uint32_t
DecodedPath::Deserialize(Buffer::Iterator start)
{
    base.Deserialize(start);

    InfoFields.clear();
    InfoFields.resize( base.NumINF);
    for (auto& infoField : InfoFields)
    {
        infoField.Deserialize(start);
    }

    HopFields.clear();
    HopFields.resize(base.NumHops);
    for (auto& hf : HopFields)
    {
        hf.Deserialize(start);
    }
    return HOPLEN;
}
*/
void
DecodedPath::Serialize( buffer_iterator start) const
{
NS_ASSERT_MSG( start.GetRemainingSize() >= Len() ,
 "not enough buffer space to serialize DecodedPath" );

 base.PathMeta.Serialize(start);
 start+=base.PathMeta.GetSerializedSize();

    for (const auto& info : InfoFields)
    {
        info.Serialize(start);
        start += INFO_FIELD_LEN;
    }

    for ( const auto& hop : HopFields)
    {
        hop.Serialize(start);
        start += HOPLEN;
    }
}

std::expected<uint32_t,error>
DecodedPath::Deserialize( const_buffer_iterator start)
{

    uint32_t bytes;
    if( auto tmp = base.Deserialize(start); tmp)
    {bytes = *tmp;
    }else
    {
        return std::unexpected( tmp.error() );
    }

// vielleicht muss die start+= *tmp zeile  hinter den '< minLen' check verschoben werden
    
    	if(auto minLen = Len(); start.GetRemainingSize() < minLen )
        {
		return std::unexpected( basic_error{"DecodedPath raw too short",
         "expected", std::to_string(minLen),
          "actual", std::to_string( start.GetRemainingSize() )
          } );
	}
    
    start += bytes;
    InfoFields.clear();
    InfoFields.resize( base.NumINF);
    for (auto& infoField : InfoFields)
    {
        infoField.Deserialize(start);
        start+= INFO_FIELD_LEN;
    }

    HopFields.clear();
    HopFields.resize(base.NumHops);
    for (auto& hf : HopFields)
    {
        hf.Deserialize(start);
        start += HOPLEN;
    }
    //return HOPLEN;
    return base.Len();
}

RawPath DecodedPath::ToRaw() const
{
neo::bytes buff{ Len() };

auto iter = neo::bytewise_iterator(neo::as_buffer( buff ) );

Serialize( iter );

//RawPath raw;
//raw.Deserialize( iter );

RawPath praw{base, std::move(buff) };

return praw;
}

/*
// ToRaw tranforms scion.Decoded into scion.Raw.
func (s *Decoded) ToRaw() (*Raw, error) {
	b := make([]byte, s.Len())
	if err := s.SerializeTo(b); err != nil {
		return nil, err
	}
	raw := &Raw{}
	if err := raw.DecodeFromBytes(b); err != nil {
		return nil, err
	}
	return raw, nil
}*/

}; // namespace ns3
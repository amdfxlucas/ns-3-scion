#include "ns3/base-field.h"
#include <format>
#include "ns3/basic-error.h"

namespace ns3
{

	void Base::Serialize( buffer_iterator start ) const
	{
		PathMeta.Serialize( start );
	}



std::expected<uint32_t,error> Base::Deserialize( const_buffer_iterator start )
{
	// PathMeta takes care of bounds check.
	if( auto tmp = PathMeta.Deserialize( start ); !tmp)
	{
		return std::unexpected( tmp.error() );
	}
	
	NumINF = 0;
	NumHops = 0;
	for ( int i = 2; i >= 0; i-- )
    {
		if (PathMeta.SegLen[i] == 0 && NumINF > 0 )
        {
		 return std::unexpected( proto_error{ 
			std::format("Meta.SegLen[{}] == 0, but Meta.SegLen[{}] > 0", i, NumINF-1) }
		 );
		}
		if ( PathMeta.SegLen[i] > 0 && NumINF == 0 )
        {
			NumINF = static_cast<uint8_t>(i) + 1;
		}
		NumHops += PathMeta.SegLen[i];

		
	}

return GetSerializedSize();
}


    // IsXover returns whether we are at a crossover point. This includes
// all segment switches, even over a peering link. Note that handling
// of a regular segment switch and handling of a segment switch over a
// peering link are fundamentally different. To distinguish the two,
// you will need to extract the information from the info field.
bool Base::IsXover() const
{
	return PathMeta.CurrHF + 1 < uint8_t(NumHops) &&
		PathMeta.CurrINF != infIndexForHF(PathMeta.CurrHF + 1 );
}

// IsFirstHopAfterXover returns whether this is the first hop field after a crossover point.
bool Base::IsFirstHopAfterXover() const
{
	return PathMeta.CurrINF > 0 && PathMeta.CurrHF > 0 &&
		PathMeta.CurrINF-1 == infIndexForHF(PathMeta.CurrHF-1);
}

uint8_t Base::infIndexForHF( uint8_t hf)  const
{

	if( hf < PathMeta.SegLen[0])
		return 0;
	if( hf < PathMeta.SegLen[0] + PathMeta.SegLen[1] )
		return 1;
	else
		return 2;

}

error Base::IncPath()
{
    if( NumINF == 0 )
    {
      return basic_error{"empty path cannot be increased"};
    }

   if( PathMeta.CurrHF >= NumHops-1 )
   {
		PathMeta.CurrHF = NumHops - 1;
		return basic_error{"path already at end",
								"curr_hf",std::to_string( PathMeta.CurrHF),
								"num_hops", std::to_string(NumHops) };
	}
	PathMeta.CurrHF++;
	// Update CurrINF
	PathMeta.CurrINF = infIndexForHF(PathMeta.CurrHF);

	return {};
}

MetaHdr::operator std::string( ) const
{
    
    return std::format("[ CurrInf: {}, CurrHF: {}, SegLen: {},{},{} ]", CurrINF, CurrHF, SegLen[0],SegLen[1],SegLen[2] );
	

}

// populates the fields from a raw buffer.
// The buffer must be of length >= scion.MetaLen.
std::expected<uint32_t,error> MetaHdr::Deserialize( const_buffer_iterator start )
{
	if( auto actual = start.GetRemainingSize(); actual < METALEN )
	{
		return std::unexpected( basic_error{ "insufficient buffer ",
											"expected" , "4",
											"actual", std::to_string(actual)

		 } );
	}

	auto line = start.ReadNtohU32();
	CurrINF = uint8_t(line >> 30);
	CurrHF = uint8_t(line>>24) & 0x3F;
	SegLen[0] = uint8_t(line>>12) & 0x3F;
	SegLen[1] = uint8_t(line>>6) & 0x3F;
	SegLen[2] = uint8_t(line) & 0x3F;
return METALEN;
}


// writes the fields into the provided buffer.
 //The buffer must be of length >= scion.MetaLen.
void MetaHdr::Serialize( buffer_iterator start )const
{
	NS_ASSERT_MSG( start.GetRemainingSize()>= GetSerializedSize() ,
	 "Not Enough buffer space to serialize MetaHdr");
	 
	uint32_t line = uint32_t(CurrINF)<<30 | uint32_t(CurrHF&0x3F)<<24;
	line |= uint32_t(SegLen[0]&0x3F) << 12;
	line |= uint32_t(SegLen[1]&0x3F) << 6;
	line |= uint32_t(SegLen[2] & 0x3F);
	start.WriteHtonU32( line );
}

}
#include "ns3/one-hop-path.h"
#include "ns3/scion-path.h"
#include "ns3/buffer.h"
#include "ns3/decoded-path.h"

namespace ns3
{
// Rerverse a OneHop path that returns a reversed SCION path.
    void OneHopPath::Reverse()
    {

	auto d = ConvertToDecoded( *this);
	

	// increment the path, since we are at the receiver side.
	d.IncPath();
	
    d.Reverse();
	
    std::swap( *this, ConvertFrom( d ) );
    }
    
// ToSCIONDecoded converts the one hop path in to a normal SCION path in the
// decoded format.
DecodedPath OneHopPath::ConvertToDecoded( const OneHopPath& path)
{
	 NS_ASSERT_MSG( path._secondHop.ConsIngress != 0 
     ,"incomplete OneHop path can't be converted");
	
	}
    DecodedPath d;
    Base b { .PathMeta={.SegLen={2,0,0}},.NumINF=1 ,.NumHops=2  };
    d.base = std::move(b);

    InfoField inf{ .ConsDir=true, .SegID= path.Info.SegID ,.Timestamp= path.Info.Timestamp };
    d.InfoFields.emplace_back(std::move(inf ));

    HopField hf1{ .IngressRouterAlert= path._firstHop.IngressRouterAlert ,
                  .EgressRouterAlert=path._firstHop.EgressRouterAlert,
                   .ExpTime = path._firstHop.ExpTime,
                  .ConsIngress = path._firstHop.ConsIngress,
                  .ConsEgress = path._firstHop.ConsEgress,                  
                   .Mac = path._firstHop.Mac
                };
    HopField hf2{ .IngressRouterAlert= path._secondHop.IngressRouterAlert ,
                  .EgressRouterAlert=path._secondHop.EgressRouterAlert,
                   .ExpTime = path._secondHop.ExpTime,
                  .ConsIngress = path._secondHop.ConsIngress,
                  .ConsEgress = path._secondHop.ConsEgress,                  
                   .Mac = path._secondHop.Mac
                };
    d.HopFields.emplace_back( std::move(hf1));
    d.HopFields.emplace_back( std::move(hf2));

    return d;
	
	
}

     Path OneHopPath::ConvertTo(const OneHopPath& )
     {
        OneHopPath tmp{dp};
        return Path( std::move(tmp) );
     }

	 OneHopPath OneHopPath::ConvertFrom(const Path& path )
     {
        NS_ASSERT( path.Type() == pathType_t::OneHopPath
                || path.Type()== pathType_t::RawPath );

        Buffer buff;
        path.Serialize(buff.Begin() );
        OneHopPath dp;
        dp.Deserialize(buff.Begin());
        return dp;
     }

      uint32_t OneHopPath::Deserialize( Buffer::Iterator start )
      {
        _info.Deserialize( start );
        _firstHop.Deserialize( start );
        _secondHop.Deserialize( start );
      }

    void OneHopPath::Serialize( Buffer::Iterator start ) const
    {
        _info.Serialize(start );
        _firstHop.Serialize( start );
        _secondHop.Serialize( start );
    }
}
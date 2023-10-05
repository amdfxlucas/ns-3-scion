#include "ns3/scion-svc-address.h"

namespace ns3
{

    // ParseSVC returns the SVC address corresponding to str. For anycast
// SVC addresses, use CS_A and DS_A; shorthand versions without
// the _A suffix (e.g., CS) also return anycast SVC addresses. For multicast,
// use CS_M, and DS_M.
    SVC_t::SVC_t(  std::string s )
    {
        _svc = SvcNone;
        if( s.ends_with("_A") )
        {
            s.remove_suffix( 2) ;  
        }else if ( s.ends_with("_M") )
        {
            s.remove_suffix(2);
            _svc = SVCMcast;
        }

        if( s == "DS" )
        {
            _svc |= SvcDS;
        }else if( s ==  "CS" )
        {
            _svc |= SvcCS;
        } else if ( s == "Wildcard" )
        {
            _svc |= SvcWildcard;
        }

    }

    bool SVC_t::IsMulticast()const
    {
        return (_svc & SVCMcast );
    }

    SVC_t SVC_t::Base() const
    {
        return (_svc & ^SVCMcast);
    }

    SVC_t SVC_t::Multicast() const
    {
        return _svc | SVCMcast;
    }

    SVC_t::operator std::string() const
    {
        auto s = BaseString();
        if( IsMulticast() )
        {
            s.append( "_M" );
        }
        return s;
    }

    std::string SVC_t::BaseString() const
    {
        switch( Base()._svc )
        {
            case SvcsDS:
                return "DS";
            case SvcCS:
                return "CS";
            case SvcWildcard:
                return "Wildcard";
            default:
            {
                std::stringstream ss;
                ss << "<SVC:0x"<< std::hex << std::setfill("0") << std::setw(4);
                ss << _svc;
                return ss.str();
            }
        };
    }


    SVCAddress::SVCAddress( const std::string & s)
    : _svc( s)
    {

    }

    bool SVCAddress::IsMulticast()const
    {return _svc.IsMulticast(); }

    SVC_t SVCAddress::Base()const
    {return _svc.Base();}

    SVC_t SVCAddress::Multicast() const
    {
        return _svc.Multicast();
    }


}
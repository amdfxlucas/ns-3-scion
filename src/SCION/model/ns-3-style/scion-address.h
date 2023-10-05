#pragma once
/*
 * Copyright (c) 2005 INRIA
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */

#ifndef SCION_ADDRESS_H
#define SCION_ADDRESS_H

#include "ns3/address.h"
#include "ns3/attribute-helper.h"
#include "ns3/scion-types.h"
#include "ns3/buffer.h"

#include <ostream>
#include <stdint.h>

namespace ns3
{

  

uint8_t AddrTypeConv( AddrType_t t );

// inverse of AddrTypeConf
AddrType_t ConvAddrType( uint8_t t );

/**
 * \ingroup address
 *

 */
class SCIONAddress
{
  public:
    SCIONAddress() = default;
    // host may be an Ipv4 or 6 Address, Inet or Inet6SocketAddr
    explicit SCIONAddress( IA_t ia, const Address& host );
    // if host is InetSocket address, the port will get overridden
    explicit SCIONAddress( ISD_t isd, AS_t as, const Address& host, uint16_t port = 0 );
    SCIONAddress( const std::string & address);

    uint32_t GetSerializedSize() const;

    uint8_t GetLength() const;

    // SCIONAddress(const char* address);

    // void Set(const char* address);
    void Serialize(  uint8_t * buffer ) const;
    void Serialize(TagBuffer) const;
    void Serialize(Buffer::Iterator) const;
    ISD_t GetISD() const {return ISD_FROM_IA(_ia ); }
    AS_t GetAS() const { return AS_FROM_IA( _ia ); }
    uint16_t GetPort()const{return _port; }
    void SetPort( uint16_t p ) { _port = p; }
    const Address& GetHostAddress() const { return _hostAddr; }
    const IA_t& GetIA()const{return _ia; }

    SCIONAddress& Deserialize( TagBuffer);
    SCIONAddress& Deserialize( Buffer::Iterator );
    SCIONAddress& Deserialize( const uint8_t * buffer );
    
    void Print(std::ostream& os) const;

    /**
     * \return true if address is initialized (i.e., set to something), false otherwise
     */
    //bool IsInitialized() const;
    /**
     * \return true if address is 0.0.0.0; false otherwise
     */
    //bool IsAny() const;
    /**
     * \return true if address is 127.0.0.1; false otherwise
     */
    // bool IsLocalhost() const;

   static bool IsMatchingType(const Address& address);
    
    operator Address() const;

    static SCIONAddress ConvertFrom(const Address& address);

    Address ConvertTo() const;

  std::strong_ordering operator <=>( const SCIONAddress &other ) const// = default;
  {
    auto res = _ia <=> other.GetIA();
    auto hostEq = _hostAddr == other.GetHostAddress();
    auto hostLess = _hostAddr < other.GetHostAddress();
    return (res == std::strong_ordering::equal ) ?  ( hostEq ? (_port <=> other.GetPort() ) 
                                   : (hostLess ? std::strong_ordering::less
                                               : std::strong_ordering::greater ) )
                       : res  ;
  }

static uint8_t GetType();
  private:
    IA_t _ia;
    AddrType_t addrType;
    Address _hostAddr; // either an Ipv4 or Ipv6 addr. Not InetSocketAddr
    uint16_t _port=0; // or does the hostaddress i.e. Ipv6Address already have one ?!

    
    // bool m_initialized; //!<  address has been explicitly initialized to a valid value.

    /**
     * \brief Equal to operator.
     *
     * \param a the first operand.
     * \param b the first operand.
     * \returns true if the operands are equal.
     */
    //friend bool operator==(const Ipv4Address& a, const Ipv4Address& b);

    /**
     * \brief Not equal to operator.
     *
     * \param a the first operand.
     * \param b the first operand.
     * \returns true if the operands are not equal.
     */
    //friend bool operator!=(const Ipv4Address& a, const Ipv4Address& b);

    /**
     * \brief Less than to operator.
     *
     * \param a the first operand.
     * \param b the first operand.
     * \returns true if the first operand is less than the second.
     */
    //friend bool operator<(const Ipv4Address& a, const Ipv4Address& b);
};


ATTRIBUTE_HELPER_HEADER(SCIONAddress);

Address SetPort( const Address& address, uint16_t port );

/**
 * \brief Stream insertion operator.
 *
 * \param os the stream
 * \param address the address
 * \returns a reference to the stream
 */
std::ostream& operator<<(std::ostream& os, const SCIONAddress& address);

/**
 * \brief Stream extraction operator.
 *
 * \param is the stream
 * \param address the address
 * \returns a reference to the stream
 */
std::istream& operator>>(std::istream& is, SCIONAddress& address);

/*
inline bool
operator==(const SCIONAddress& a, const Ipv4Address& b)
{
    return (a.m_address == b.m_address);
}

inline bool
operator!=(const Ipv4Address& a, const Ipv4Address& b)
{
    return (a.m_address != b.m_address);
}

inline bool
operator<(const Ipv4Address& a, const Ipv4Address& b)
{
    return (a.m_address < b.m_address);
}
*/



} // namespace ns3

namespace std
{
template <>
struct hash<ns3::SCIONAddress>
{
    auto operator()(const ns3::SCIONAddress& a) const -> size_t
    {
      std::stringstream ss;
      a.Print(ss);
      return std::hash<std::string>{}(ss.str() );
        //  return hash<SNETPath>{}(xyz.value);
    }
};
}

#endif // SCION_ADDRESS
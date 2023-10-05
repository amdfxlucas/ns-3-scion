#pragma once
#include "ns3/address.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"

#include <string>
#include <variant>
#include <optional>
namespace ns3
{
/* an address /port pair
can convert either to InetSocketAddress or Inet6SocketAddress
*/
class L4Address
{
    template <typename... Functions>
    struct overload : Functions...
    {
        using Functions::operator()...;

        overload(Functions... functions)
            : Functions(functions)...
        {
        }
    };

  public:
  //  L4Address(const std::string&);
  L4Address() = default;

    L4Address(const InetSocketAddress& a)
        : m_addr(a)
    {
    }

    L4Address(const Inet6SocketAddress& a)
        : m_addr(a)
    {
    }
    L4Address( const Address& a)
    {
        if( InetSocketAddress::IsMatchingType(a))
        {
            m_addr = InetSocketAddress::ConvertFrom(a);
        } else if (Inet6SocketAddress::IsMatchingType(a))
        {
            m_addr = Inet6SocketAddress::ConvertFrom(a);
        }
        
    }

    // returns the stored IP address without port
     Address GetIP() const
    {
        return std::visit(
            overload([](const InetSocketAddress& a) -> Address { return a.GetIpv4(); },
                     [](const Inet6SocketAddress& a) -> Address { return a.GetIpv6(); }

                     ),
            m_addr.value());
    }

    uint16_t GetPort() const
    {
        return std::visit(
            overload([](const InetSocketAddress& a) -> uint16_t { return a.GetPort(); },
                     [](const Inet6SocketAddress& a) -> uint16_t { return a.GetPort(); }

                     ),
            m_addr.value());
    }

    /**
     * \param address address to test
     * \returns true if the address matches, false otherwise.
     */
    // static bool IsMatchingType(const Address& address);

    /**
     * \returns an Address instance which represents this
     * InetSocketAddress instance.
     */
    operator Address() const
    {
        return std::visit(
            overload([](const InetSocketAddress& a) -> Address { return a.ConvertTo(); },
                     [](const Inet6SocketAddress& a) -> Address { return a.ConvertTo(); }

                     ),
            m_addr.value());
    }

    /**
     * \brief Returns an InetSocketAddress which corresponds to the input
     * Address.
     *
     * \param address the Address instance to convert from.
     * \returns an InetSocketAddress
     */
   // static L4Address ConvertFrom(const Address& address);


L4Address& operator=( const Address& a )
{
    if( InetSocketAddress::IsMatchingType( a ) )
    {
        m_addr = InetSocketAddress::ConvertFrom(a);
    } else if( Inet6SocketAddress::IsMatchingType(a) )
    {
        m_addr = Inet6SocketAddress::ConvertFrom(a);
    }
    return *this;
}
    /**
     * \brief Convert to an Address type
     * \return the Address corresponding to this object.
     */
    Address ConvertTo() const
    {
        return std::visit(
            overload([](const InetSocketAddress& a) -> Address { return a.ConvertTo(); },
                     [](const Inet6SocketAddress& a) -> Address { return a.ConvertTo(); }

                     ),
            m_addr.value() );
    }

  private:
    std::optional< std::variant<InetSocketAddress, Inet6SocketAddress> > m_addr = std::nullopt;
};
} // namespace ns3
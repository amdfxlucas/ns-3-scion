/*
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
 * Author: Faker Moatamri <faker.moatamri@sophia.inria.fr>
 *
 */
/**
 * This is the test code for ipv4-l3-protocol.cc
 */

#include "ns3/arp-l3-protocol.h"
#include "ns3/inet-socket-address.h"
#include "ns3/ipv4-interface.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/log.h"
#include "ns3/loopback-net-device.h"
#include "ns3/node.h"
#include "ns3/scion-address.h"
#include "ns3/simulator.h"
#include "ns3/test.h"

#include <compare>

using namespace ns3;

/**
 * \ingroup internet-test
 *
 * \brief IPv4 Test
 */
class SCIONAddressTestCase : public TestCase
{
  public:
    SCIONAddressTestCase();
    ~SCIONAddressTestCase() override;
    void DoRun() override;
};

SCIONAddressTestCase::SCIONAddressTestCase()
    : TestCase("Verify the IPv4 layer 3 protocol")
{
}

SCIONAddressTestCase::~SCIONAddressTestCase()
{
}

void
SCIONAddressTestCase::DoRun()
{
   
    {
        SCIONAddress a0{"19-ffaa:0:1067,127.0.0.1:8080"};
        {
            std::stringstream out;
            out << a0;

            std::string str;
            out >> str;

            NS_TEST_ASSERT_MSG_EQ(
                a0.GetHostAddress().GetType(),
                Ipv4Address::GetType(),
                "HostAddress parse failed for: 19-ffaa:0:1067,192.127.0.0.1:8080");
            NS_TEST_ASSERT_MSG_EQ(
                str,
                "19-ffaa:0:1067,127.0.0.1:8080",
                "Address-To-String failed for: 19-ffaa:0:1067,192.127.0.0.1:8080");
        }
        {
            std::stringstream out;
            SCIONAddress a1;
            out << a0;
            out >> a1;

            [[maybe_unused]] auto res = a0 <=> a1;
            auto resres = res == std::strong_ordering::equal;
            NS_TEST_ASSERT_MSG_EQ(
                resres,
                true,
                "Address To- and back From- stream failed for: 19-ffaa:0:1067,192.127.0.0.1:8080");

            NS_TEST_ASSERT_MSG_EQ(
                a0.GetIA(),
                a1.GetIA(),
                "Address To- and back From- stream failed for: 19-ffaa:0:1067,192.127.0.0.1:8080");
            NS_TEST_ASSERT_MSG_EQ(
                a0.GetAS(),
                a1.GetAS(),
                "Address To- and back From- stream failed for: 19-ffaa:0:1067,192.127.0.0.1:8080");
            NS_TEST_ASSERT_MSG_EQ(
                a0.GetISD(),
                a1.GetISD(),
                "Address To- and back From- stream failed for: 19-ffaa:0:1067,192.127.0.0.1:8080");
            NS_TEST_ASSERT_MSG_EQ(
                a0.GetHostAddress(),
                a1.GetHostAddress(),
                "Address To- and back From- stream failed for: 19-ffaa:0:1067,192.127.0.0.1:8080");
        }

        NS_TEST_ASSERT_MSG_EQ(a0.GetIA(),
                              5629130167029863,
                              "IA computation failed for: 19-ffaa:0:1067,127.0.0.1:8080");
        NS_TEST_ASSERT_MSG_EQ(a0.GetISD(), 19, "ISD computation failed for 19-ffaa:0:1067");
        NS_TEST_ASSERT_MSG_EQ(a0.GetPort(),
                              8080,
                              "Port computation failed for: 19-ffaa:0:1067,127.0.0.1:8080");
        NS_TEST_ASSERT_MSG_EQ(a0.GetHostAddress(),
                              Ipv4Address("127.0.0.1"),
                              "HostAddress computation failed for: 19-ffaa:0:1067,127.0.0.1:8080");
        NS_TEST_ASSERT_MSG_EQ(a0.GetAS(),
                              281105609527399,
                              "AS computation failed for ffaa:0:1067! was: " << a0.GetAS());
    }


{
    SCIONAddress a0{"19-ffaa:0:1067,127.0.0.1:8080"};

    NS_ASSERT_MSG( Ipv4Address::GetType() == a0.GetHostAddress().GetType(), "AddressType wrong");

    NS_TEST_ASSERT_MSG_EQ( ConvAddrType( Ipv4Address::GetType() ), AddrType_t::T4Ip , "ConvAddrType not working correctly");
    NS_TEST_ASSERT_MSG_EQ(  Ipv4Address::GetType() ,AddrTypeConv( AddrType_t::T4Ip ), "AddrTypeConv not working correctly");

    Buffer buf;
    [[maybe_unused]]auto iter = buf.Begin();
    // a0.Serialize(iter );

    uint8_t buffer[100];
    a0.Serialize( buffer );

    SCIONAddress a1;
    a1.Deserialize( buffer );

    [[maybe_unused]] auto res = a0 <=> a1;
            auto resres = res == std::strong_ordering::equal;
            NS_TEST_ASSERT_MSG_EQ(
                resres,
                true,
                "Address Serialization/Deserialization failed for: 19-ffaa:0:1067,192.127.0.0.1:8080");
               
  NS_TEST_ASSERT_MSG_EQ(
                a0.GetIA(),
                a1.GetIA(),
                "Address To- and back From- stream failed for: 19-ffaa:0:1067,192.127.0.0.1:8080");
            NS_TEST_ASSERT_MSG_EQ(
                a0.GetAS(),
                a1.GetAS(),
                "Address To- and back From- stream failed for: 19-ffaa:0:1067,192.127.0.0.1:8080");
            NS_TEST_ASSERT_MSG_EQ(
                a0.GetISD(),
                a1.GetISD(),
                "Address To- and back From- stream failed for: 19-ffaa:0:1067,192.127.0.0.1:8080");
            NS_TEST_ASSERT_MSG_EQ(
                a0.GetHostAddress(),
                a1.GetHostAddress(),
                "Address To- and back From- stream failed for: 19-ffaa:0:1067,192.127.0.0.1:8080");
             NS_TEST_ASSERT_MSG_EQ(
                a0.GetPort(),
                a1.GetPort(),
                "Address To- and back From- stream failed for: 19-ffaa:0:1067,192.127.0.0.1:8080");
        
{     
    
    SCIONAddress a0{"19-ffaa:0:1067,[2001:db8::2:1]:8080"};

    // NS_ASSERT_MSG(  Ipv4Address::IsMatchingType( a0.GetHostAddress() ),"AddressType wrong" );

    NS_TEST_ASSERT_MSG_EQ( Ipv6Address::GetType() , a0.GetHostAddress().GetType(), "AddressType wrong");

        NS_TEST_ASSERT_MSG_EQ( ConvAddrType( Ipv6Address::GetType() ), AddrType_t::T16Ip , "ConvAddrType not working correctly");
    NS_TEST_ASSERT_MSG_EQ(  Ipv6Address::GetType() ,AddrTypeConv( AddrType_t::T16Ip ), "AddrTypeConv not working correctly");

    Buffer buf;
    [[maybe_unused]]auto iter = buf.Begin();
    // a0.Serialize(iter );

    uint8_t buffer[100];
    a0.Serialize( buffer );

    SCIONAddress a1;
    a1.Deserialize( buffer );

            [[maybe_unused]] auto res = a0 <=> a1;
            auto resres = res == std::strong_ordering::equal;
            NS_TEST_ASSERT_MSG_EQ(
                resres,
                true,
                "Address To- and back From- stream failed for: 19-ffaa:0:1067,192.127.0.0.1:8080");}
}

    //  Simulator::Destroy();
}

/**
 * \ingroup internet-test
 *
 * \brief IPv4 TestSuite
 */
class SCIONAddressTestSuite : public TestSuite
{
  public:
    SCIONAddressTestSuite()
        : TestSuite("SCIONAddress", UNIT)
    {
        AddTestCase(new SCIONAddressTestCase(), TestCase::QUICK);
    }
};

static SCIONAddressTestSuite g_TestSuite; //!< Static variable for test initialization

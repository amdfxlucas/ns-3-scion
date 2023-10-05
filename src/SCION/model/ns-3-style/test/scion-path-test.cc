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
#include "ns3/scion-path.h"
#include "ns3/decoded-path.h"
#include "ns3/simulator.h"
#include "ns3/test.h"

#include <compare>

using namespace ns3;
/*

*/
class SCIONPathTestCase : public TestCase
{
  public:
    SCIONPathTestCase();
    ~SCIONPathTestCase() override;
    void DoRun() override;
};

SCIONPathTestCase::SCIONPathTestCase()
    : TestCase("Verify SCION Path impl")
{
}

SCIONPathTestCase::~SCIONPathTestCase()
{
}

void
SCIONPathTestCase::DoRun()
{

 
DecodedPath p1;
DecodedPath p2;

Path pp{ std::move(p1) };

//Path ppp{p1};

Buffer buffer1{};
buffer1.AddAtStart(pp.Len() );


pp.Serialize( buffer1.Begin() );



 //NS_TEST_ASSERT_MSG_EQ(ifaceAddr4, output, "The addresses should be identical");

    //  Simulator::Destroy();
}

/**
 * \ingroup internet-test
 *
 * \brief IPv4 TestSuite
 */
class SCIONPathTestSuite : public TestSuite
{
  public:
    SCIONPathTestSuite()
        : TestSuite("SCIONPath", UNIT)
    {
        AddTestCase(new SCIONPathTestCase(), TestCase::QUICK);
    }
};

static SCIONPathTestSuite g_pathTestSuite; //!< Static variable for test initialization

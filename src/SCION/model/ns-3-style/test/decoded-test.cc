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

#include "neo/byte_array.hpp"
#include "neo/bytes.hpp"

#include "ns3/decoded-path.h"
#include "ns3/hop-field.h"
#include "ns3/info-field.h"
#include "ns3/log.h"
#include "ns3/raw-path.h"
#include "ns3/scion-address.h"
#include "ns3/scion-path.h"
#include "ns3/simulator.h"
#include "ns3/test.h"

#include "scion-test-data.h"

#include <compare>
#include <expected>

namespace ns3
{

/*
// Copyright 2020 Anapaya Systems
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.






var emptyDecodedTestPath = &scion.Decoded{
    Base:       scion.Base{},
    InfoFields: []path.InfoField{},
    HopFields:  []path.HopField{},
}




func TestDecodedSerializeDecode(t *testing.T) {
    b := make([]byte, decodedTestPath.Len())
    assert.NoError(t, decodedTestPath.SerializeTo(b))
    s := &scion.Decoded{}
    assert.NoError(t, s.DecodeFromBytes(b))
    assert.Equal(t, decodedTestPath, s)
}


func TestEmptyDecodedReverse(t *testing.T) {
    _, err := emptyDecodedTestPath.Reverse()
    assert.Error(t, err)
}


*/
class DecodedPathTestCase : public TestCase, public SCIONTestData
{
  public:
    DecodedPathTestCase();
    ~DecodedPathTestCase() override;
    void DoRun() override;
    void DecodedFromTo();

    void InfoFieldTest();
    void HopFieldTest();

    void FieldTest();

    void DecodedToRaw();
    void TestDecodedReverse();

  private:
};

void
DecodedPathTestCase::TestDecodedReverse()
{
    /*

func TestDecodedReverse(t *testing.T) {
    for name, tc := range pathReverseCases {
        name, tc := name, tc
        for i := range tc.inIdxs {
            i := i
            t.Run(fmt.Sprintf("%s case %d", name, i+1), func(t *testing.T) {
                t.Parallel()
                inputPath := mkDecodedPath(t, tc.input, uint8(tc.inIdxs[i][0]),
uint8(tc.inIdxs[i][1])) wantPath := mkDecodedPath(t, tc.want, uint8(tc.wantIdxs[i][0]),
uint8(tc.wantIdxs[i][1])) revPath, err := inputPath.Reverse() assert.NoError(t, err) assert.Equal(t,
wantPath, revPath)
            })
        }
    }
}
    */

    for (const auto& [k, v] : pathReverseCases)
    {
        for ( size_t i = 0; i < v.inIdxs.size(); ++i)
        {
            auto inputPath = mkDecodedPath(v.input, v.inIdxs[i].first, v.inIdxs[i].second);

            auto wantPath = mkDecodedPath(v.want, v.wantIdxs[i].first, v.wantIdxs[i].second);

            auto revPath = inputPath.Reverse();

            NS_TEST_ASSERT_MSG_EQ(wantPath, revPath, "DecodedPath::Reverse "<< k << " case " << std::to_string(i+1) );
        }
    }
}

/* // moved to SCIONTestData
DecodedPath
DecodedPathTestCase::mkDecodedPath(DecodedPathTestCase::pathCase pcase,
                                   uint8_t infIdx,
                                   uint8_t hopIdx)
{

    DecodedPath s;
    MetaHdr meta{.CurrINF = infIdx, .CurrHF = hopIdx};

    for (int i = 0; const auto& dir : pcase.infos)
    {
        s.InfoFields.push_back( InfoField{.ConsDir = dir} );
        meta.SegLen[i] = pcase.hops[i].size();
        ++i;
    }

    int i = 0;
    for (const auto& hops : pcase.hops)
    {
        for (const auto& hop : hops)
        {
            s.HopFields.push_back( HopField{.ConsIngress = hop, .ConsEgress = hop} );
            ++i;
        }
    }
    s.base.PathMeta = meta;
    s.base.NumINF = pcase.infos.size();
    s.base.NumHops = i;
    return s;
}*/

void
DecodedPathTestCase::DecodedToRaw()
{
    /*
    func TestDecodedToRaw(t *testing.T) {
    raw, err := decodedTestPath.ToRaw()
    assert.NoError(t, err)
    assert.Equal(t, rawTestPath, raw)
    */

    auto raw = decodedTestPath.ToRaw();

    NS_TEST_ASSERT_MSG_EQ(raw, rawTestPath, "DecodedPath::ToRaw() failed ");
}

void
DecodedPathTestCase::InfoFieldTest()
{
    for (const auto& inf : testInfoFields)
    {
        neo::bytes buff{inf.Len()};

        inf.Serialize(neo::bytewise_iterator(neo::as_buffer(buff)));

        InfoField info;

        info.Deserialize(neo::bytewise_iterator(neo::as_buffer(buff)));

        NS_TEST_ASSERT_MSG_EQ(info.Peer, inf.Peer, "InfoField::Peer");
        NS_TEST_ASSERT_MSG_EQ(info.ConsDir, inf.ConsDir, "InfoField::ConsDir");
        NS_TEST_ASSERT_MSG_EQ(info.SegID, inf.SegID, "InfoField::SegID");
        NS_TEST_ASSERT_MSG_EQ(info.Timestamp, inf.Timestamp, "InfoField::Timestamp");
    }
}

void
DecodedPathTestCase::HopFieldTest()
{
    for (const auto& hf : testHopFields)
    {
        neo::bytes buff{hf.Len()};

        hf.Serialize(neo::bytewise_iterator(neo::as_buffer(buff)));

        HopField hop;

        hop.Deserialize(neo::bytewise_iterator(neo::as_buffer(buff)));

        NS_TEST_ASSERT_MSG_EQ(hop.IngressRouterAlert,
                              hf.IngressRouterAlert,
                              "HopField::IngressRouterAlert");
        NS_TEST_ASSERT_MSG_EQ(hop.EgressRouterAlert,
                              hf.EgressRouterAlert,
                              "HopField::EgressRouterAlert");

        NS_TEST_ASSERT_MSG_EQ(hop.ExpTime, hf.ExpTime, "HopField::ExpTime");

        NS_TEST_ASSERT_MSG_EQ(hop.ConsEgress, hf.ConsEgress, "HopField::ConsEgress");
        NS_TEST_ASSERT_MSG_EQ(hop.ConsIngress, hf.ConsIngress, "HopField::ConsIngress");
    }
}

void
DecodedPathTestCase::FieldTest()
{
    neo::bytes buff{decodedTestPath.base.GetSerializedSize()};

    decodedTestPath.base.Serialize(neo::bytewise_iterator(neo::as_buffer(buff)));

    Base base;

    base.Deserialize(neo::bytewise_iterator(neo::as_buffer(buff)));

    NS_TEST_ASSERT_MSG_EQ(decodedTestPath.base.PathMeta.CurrHF,
                          base.PathMeta.CurrHF,
                          "MetaHdr::currentHopField ");
    NS_TEST_ASSERT_MSG_EQ(decodedTestPath.base.PathMeta.CurrINF,
                          base.PathMeta.CurrINF,
                          "MetaHdr::currentInfoField ");
    NS_TEST_ASSERT_MSG_EQ(decodedTestPath.base.PathMeta.SegLen[0],
                          base.PathMeta.SegLen[0],
                          "MetaHdr::SegLen[0] ");
    NS_TEST_ASSERT_MSG_EQ(decodedTestPath.base.PathMeta.SegLen[1],
                          base.PathMeta.SegLen[1],
                          "MetaHdr::SegLen[1] ");
    NS_TEST_ASSERT_MSG_EQ(decodedTestPath.base.PathMeta.SegLen[2],
                          base.PathMeta.SegLen[2],
                          "MetaHdr::SegLen[2] ");

    NS_TEST_ASSERT_MSG_EQ(decodedTestPath.base.NumINF, base.NumINF, "NumInf");
    NS_TEST_ASSERT_MSG_EQ(decodedTestPath.base.NumHops, base.NumHops, "NumHops");
}

void
DecodedPathTestCase::DecodedFromTo()
{
    /*
    func TestDecodedSerialize(t *testing.T) {
    b := make([]byte, decodedTestPath.Len())
    assert.NoError(t, decodedTestPath.SerializeTo(b))
    assert.Equal(t, rawPath, b)
    }

    func TestDecodedDecodeFromBytes(t *testing.T) {
    s := &scion.Decoded{}
    assert.NoError(t, s.DecodeFromBytes(rawPath))
    assert.Equal(t, decodedTestPath, s)
    }*/

    neo::bytes buff{decodedTestPath.Len()};

    decodedTestPath.Serialize(neo::bytewise_iterator(neo::as_buffer(buff)));

    DecodedPath dp1;
    auto res = dp1.Deserialize(neo::bytewise_iterator(neo::as_buffer(buff)) );
    NS_TEST_ASSERT_MSG_EQ( static_cast<bool>(res),true,"unexpected error" << res.error().what() );
    NS_TEST_ASSERT_MSG_EQ( res.value() ,decodedTestPath.Len(), "");

    NS_TEST_ASSERT_MSG_EQ(decodedTestPath.base.PathMeta.CurrHF,
                          dp1.base.PathMeta.CurrHF,
                          "MetaHdr::currentHopField ");
    NS_TEST_ASSERT_MSG_EQ(decodedTestPath.base.PathMeta.CurrINF,
                          dp1.base.PathMeta.CurrINF,
                          "MetaHdr::currentInfoField ");
    NS_TEST_ASSERT_MSG_EQ(decodedTestPath.base.PathMeta.SegLen[0],
                          dp1.base.PathMeta.SegLen[0],
                          "MetaHdr::SegLen[0] ");
    NS_TEST_ASSERT_MSG_EQ(decodedTestPath.base.PathMeta.SegLen[1],
                          dp1.base.PathMeta.SegLen[1],
                          "MetaHdr::SegLen[1] ");
    NS_TEST_ASSERT_MSG_EQ(decodedTestPath.base.PathMeta.SegLen[2],
                          dp1.base.PathMeta.SegLen[2],
                          "MetaHdr::SegLen[2] ");

    NS_TEST_ASSERT_MSG_EQ(decodedTestPath.base.NumINF, dp1.base.NumINF, "NumInf");
    NS_TEST_ASSERT_MSG_EQ(decodedTestPath.base.NumHops, dp1.base.NumHops, "NumHops");
    NS_TEST_ASSERT_MSG_EQ(decodedTestPath.InfoFields.size(),
                          dp1.InfoFields.size(),
                          "InfoFields.size()");
    NS_TEST_ASSERT_MSG_EQ(decodedTestPath.HopFields.size(),
                          dp1.HopFields.size(),
                          "HopFields.size()");

    for (int i = 0; const auto& hop : dp1.HopFields)
    {
        NS_TEST_ASSERT_MSG_EQ(decodedTestPath.HopFields[i], hop, "HopField[" << i << "]");
        ++i;
    }

    for (int i = 0; const auto& info : dp1.InfoFields)
    {
        NS_TEST_ASSERT_MSG_EQ(decodedTestPath.InfoFields[i], info, "InfoField[" << i << "]");
        ++i;
    }

    auto cmpare = decodedTestPath == dp1;
    NS_TEST_ASSERT_MSG_EQ(cmpare, true, "DecodedPath serialization/deserialization failed");
}

DecodedPathTestCase::DecodedPathTestCase()
    : TestCase("Verify DecodedPath impl")
{
}

DecodedPathTestCase::~DecodedPathTestCase()
{
}

void
DecodedPathTestCase::DoRun()
{
    TestDecodedReverse();
    HopFieldTest();
    InfoFieldTest();

    FieldTest();

    DecodedToRaw();

    DecodedFromTo();

    DecodedPath p1;
    DecodedPath p2;

    Path pp{std::move(p1)};

    // Path ppp{p1};

    Buffer buffer1{};
    buffer1.AddAtStart(pp.Len());

    pp.Serialize(buffer1.Begin());

    // NS_TEST_ASSERT_MSG_EQ(ifaceAddr4, output, "The addresses should be identical");

    //  Simulator::Destroy();
}

class DecodedPathTestSuite : public TestSuite
{
  public:
    DecodedPathTestSuite()
        : TestSuite("DecodedPath", UNIT)
    {
        AddTestCase(new DecodedPathTestCase(), TestCase::QUICK);
    }
};

static DecodedPathTestSuite g_pathTestSuite; //!< Static variable for test initialization

}
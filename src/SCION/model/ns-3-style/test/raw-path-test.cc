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
#include <stdint.h>
#include "ns3/go-errors.h"

#include "scion-test-data.h"

#include <compare>
#include <expected>

namespace ns3
{

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





class RawPathTestCase : public TestCase, public SCIONTestData
{
  public:
    RawPathTestCase();
    ~RawPathTestCase() override;
    void DoRun() override;

    void TestRawSerialize();
    void TestRawDecodeFromBytes();
    void TestRawSerializeDecode();
    void TestRawReverse();
    void TestRawToDecoded();
    void TestGetInfoField();
    void TestGetHopField();
    void TestLastHop();
    void TestPenultimateHop();
    void TestSetInfoField();
    void TestSetHopField();

  private:

};


void RawPathTestCase::TestSetHopField()
{
 struct _case_{ int idx; const HopField& want; bool err; };
    std::map< std::string_view, _case_> testCases{
        {"first hop", _case_{.idx=0, .want= testHopFields[3], .err=false } },
        {"second hop", _case_{.idx=2, .want= testHopFields[0], .err=false } }
    };

    /*
			raw := &scion.Raw{}
			require.NoError(t, raw.DecodeFromBytes(rawPath))

			err := raw.SetHopField(tc.want, tc.idx)
			tc.errorFunc(t, err)
			if err != nil {
				return
			}
			got, err := raw.GetHopField(tc.idx)
			require.NoError(t, err)
			assert.Equal(t, tc.want, got) */

             for( const auto &[k,v] : testCases )
   {
    RawPath raw;
    raw.Deserialize( neo::bytewise_iterator( neo::as_buffer(rawPath) ) );

    raw.SetHopField( v.want, v.idx );

    auto got = raw.GetHopField( v.idx);
   
   NS_TEST_ASSERT_MSG_EQ( !static_cast<bool>(got),v.err, "unexpected result");
    if(got)
    NS_TEST_ASSERT_MSG_EQ( v.want, *got, "SetHopField");
   }
}

void RawPathTestCase::TestSetInfoField()
{
    struct _case_{ int idx; const InfoField& want; bool err;};
    std::map< std::string_view, _case_> testCases{
        {"first info", _case_{.idx=0, .want= testInfoFields[1] , .err=false } },
        {"second info", _case_{.idx=1, .want= testInfoFields[0], .err=false } }
    };
    /*
    testCases := map[string]struct {
		idx       int
		want      path.InfoField
		errorFunc assert.ErrorAssertionFunc
	}{
		"first info": {
			idx:       0,
			want:      testInfoFields[1],
			errorFunc: assert.NoError,
		},
		"second info": {
			idx:       1,
			want:      testInfoFields[0],
			errorFunc: assert.NoError,
		},
		"out of bounds": {
			idx:       2,
			want:      path.InfoField{},
			errorFunc: assert.Error,
		},
	}

	for name, tc := range testCases {
		name, tc := name, tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			raw := &scion.Raw{}
			require.NoError(t, raw.DecodeFromBytes(rawPath))

			err := raw.SetInfoField(tc.want, tc.idx)
			tc.errorFunc(t, err)
			if err != nil {
				return
			}
			got, err := raw.GetInfoField(tc.idx)
			require.NoError(t, err)
			assert.Equal(t, tc.want, got)
		})
	}
    */

   for( const auto &[k,v] : testCases )
   {
    RawPath raw;
    raw.Deserialize( neo::bytewise_iterator( neo::as_buffer(rawPath) ) );

    raw.SetInfoField( v.want, v.idx );

    auto got = raw.GetInfoField( v.idx);
    NS_TEST_ASSERT_MSG_EQ( !static_cast<bool>(got), v.err, "unexpected result ");

    if(got)
        NS_TEST_ASSERT_MSG_EQ( v.want, *got, "SetInfoField");
   }
}


void RawPathTestCase::TestLastHop()
{
/*
testCases := map[*scion.Raw]bool{
		createScionPath(0, 2): false,
		createScionPath(1, 2): true,
		createScionPath(2, 2): false,
		createScionPath(5, 7): false,
		createScionPath(6, 7): true,
		createScionPath(7, 7): false,
	}
	for scionRaw, want := range testCases {
		got := scionRaw.IsLastHop()
		assert.Equal(t, want, got)
	}
*/

std::map< std::pair<uint8_t, uint8_t>, bool> testCases{ 
    { {0,2}, false },
    { {1,2}, true },
    { {2,2}, false } ,
    { {5,7},false } ,
    { {6,7}, true} ,
    { {7,7}, false }
    };

for( const auto& [k, want] : testCases )
{
    auto path = createScionPath( k.first, k.second );
    auto got = path.IsLastHop();
    NS_TEST_ASSERT_MSG_EQ( want, got, "TestLastHop " );

}
}

void RawPathTestCase::TestPenultimateHop()
{
std::map< std::pair<uint8_t, uint8_t>, bool> testCases{ 
    { {0,2}, true },
    { {1,2}, false },
    { {2,2}, false } ,
    { {5,7}, true } ,
    { {6,7}, false } ,
    { {7,7}, false }
    };

for( const auto& [k, want] : testCases )
{
    auto path = createScionPath( k.first, k.second );
    auto got = path.IsPenultimateHop();
    NS_TEST_ASSERT_MSG_EQ( want, got, "TestPenultimateHop " );

}

}

/*
RawPath RawPathTestCase::createScionPath( uint8_t currHf, uint8_t NumHops )
{
    
//    scionRaw := &scion.Raw{
//		Base: scion.Base{
//			PathMeta: scion.MetaHdr{
//				CurrHF: currHF,
//			},
//			NumHops: numHops,
	//	},
	//}
    
return RawPath{  Base{ .PathMeta =MetaHdr{ .CurrHF = currHf },
                                .NumHops =NumHops   } };
}
*/

void RawPathTestCase::TestGetInfoField()
{
    struct _case_{int idx; const InfoField& want; bool err; };

    std::map<std::string_view, _case_> testCases{
        { "first info", _case_{ .idx = 0, .want= testInfoFields[0] , .err=false  }     },
        { "second info", _case_{ .idx = 1, .want= testInfoFields[1], .err=false  }     }
    };

    /*testCases := map[string]struct {
		idx       int
		want      path.InfoField
		errorFunc assert.ErrorAssertionFunc
	}{
		"first info": {
			idx:       0,
			want:      testInfoFields[0],
			errorFunc: assert.NoError,
		},
		"second info": {
			idx:       1,
			want:      testInfoFields[1],
			errorFunc: assert.NoError,
		},
		"out of bounds": {
			idx:       2,
			want:      path.InfoField{},
			errorFunc: assert.Error,
		},
	}

	for name, tc := range testCases {
		name, tc := name, tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			got, err := rawTestPath.GetInfoField(tc.idx)
			tc.errorFunc(t, err)
			assert.Equal(t, tc.want, got)
		})
	}
    */

   for( const auto&[k,v] : testCases )
   {
    auto got = rawTestPath.GetInfoField( v.idx );

    NS_TEST_ASSERT_MSG_EQ( !static_cast<bool>(got), v.err , "unexpected result");

    if(got)
    NS_TEST_ASSERT_MSG_EQ( v.want, *got , "RawPath::GetInfoField");
   }
}

void RawPathTestCase::TestRawToDecoded()
{
    /*
    	decoded, err := rawTestPath.ToDecoded()
	assert.NoError(t, err)
	assert.Equal(t, decodedTestPath, decoded)
    */

   DecodedPath decoded = rawTestPath.ConvertToDecoded();

   NS_TEST_ASSERT_MSG_EQ( decodedTestPath, decoded, "TestRawToDecoded" );
}

void
RawPathTestCase::TestRawReverse()
{
    /*
func TestRawReverse(t *testing.T) {
	for name, tc := range pathReverseCases {
		name, tc := name, tc
		for i := range tc.inIdxs {
			i := i
			t.Run(fmt.Sprintf("%s case %d", name, i+1), func(t *testing.T) {
				t.Parallel()
				input := mkRawPath(t, tc.input, uint8(tc.inIdxs[i][0]), uint8(tc.inIdxs[i][1]))
				want := mkRawPath(t, tc.want, uint8(tc.wantIdxs[i][0]), uint8(tc.wantIdxs[i][1]))
				revPath, err := input.Reverse()
				assert.NoError(t, err)
				assert.Equal(t, want, revPath)
			})
		}
	}
}
    */

    for (const auto& [k, v] : pathReverseCases)
    {
        for ( size_t i = 0; i < v.inIdxs.size(); ++i)
        {
            auto inputPath = mkRawPath(v.input, v.inIdxs[i].first, v.inIdxs[i].second);

            auto wantPath = mkRawPath(v.want, v.wantIdxs[i].first, v.wantIdxs[i].second);

            inputPath.Reverse();

            NS_TEST_ASSERT_MSG_EQ(wantPath, inputPath, "DecodedPath::Reverse "<< k << " case " << std::to_string(i+1) );
        }
    }
}

void RawPathTestCase::TestGetHopField()
{
    struct _case_{ int idx; const HopField& want; bool err;};
    std::map< std::string_view, _case_> testCases{
        { "first hop", _case_{ .idx=0, .want = testHopFields[0], .err=false } },
        { "third hop", _case_{ .idx=2, .want = testHopFields[2], .err=false } }

    };

    /*
    	testCases := map[string]struct {
		idx       int
		want      path.HopField
		errorFunc assert.ErrorAssertionFunc
	}{
		"first hop": {
			idx:       0,
			want:      testHopFields[0],
			errorFunc: assert.NoError,
		},
		"third hop": {
			idx:       2,
			want:      testHopFields[2],
			errorFunc: assert.NoError,
		},
		"out of bounds": {
			idx:       4,
			errorFunc: assert.Error,
		},
	}

	for name, tc := range testCases {
		name, tc := name, tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			got, err := rawTestPath.GetHopField(tc.idx)
			tc.errorFunc(t, err)
			assert.Equal(t, tc.want, got)
		})
	}
    */
    for( const auto&[k,v] : testCases )
   {
    auto got = rawTestPath.GetHopField( v.idx );
    NS_TEST_ASSERT_MSG_EQ( !static_cast<bool>(got), v.err , "unexpected result");

    if(got)
    NS_TEST_ASSERT_MSG_EQ( v.want, *got , "RawPath::GetHopField" );
   }
}

void RawPathTestCase::TestRawSerializeDecode()
{
    /*
    	b := make([]byte, rawTestPath.Len())
	assert.NoError(t, rawTestPath.SerializeTo(b))
	s := &scion.Raw{}
	assert.NoError(t, s.DecodeFromBytes(b))
	assert.Equal(t, rawTestPath, s)
    */

   neo::bytes buff{ rawTestPath.Len() };

   rawTestPath.Serialize( neo::bytewise_iterator( neo::as_buffer(buff) ) );

   RawPath s;

   s.Deserialize( neo::bytewise_iterator( neo::as_buffer(buff) ) );

   NS_TEST_ASSERT_MSG_EQ( rawTestPath, s, " RawSerializeDecode" );
}

void RawPathTestCase::TestRawDecodeFromBytes()
{
    /*s := &scion.Raw{}
	assert.NoError(t, s.DecodeFromBytes(rawPath))
	assert.Equal(t, rawTestPath, s)
    */

   RawPath s;
   s.Deserialize( neo::bytewise_iterator( neo::as_buffer(rawPath) ) );

   NS_TEST_ASSERT_MSG_EQ( rawTestPath, s,"RawPath::Deserialize");
}

RawPathTestCase::RawPathTestCase()
    : TestCase("Verify RawPath impl")
{
}

RawPathTestCase::~RawPathTestCase()
{
}

void RawPathTestCase::TestRawSerialize()
{
    /*	b := make([]byte, rawTestPath.Len())
	assert.NoError(t, rawTestPath.SerializeTo(b))
	assert.Equal(t, rawPath, b)
    */

   neo::bytes buff{ rawTestPath.Len() };

   rawTestPath.Serialize( neo::bytewise_iterator( neo::as_buffer(buff) ) );

    NS_TEST_ASSERT_MSG_EQ( std::ranges::equal( neo::bytewise_iterator( neo::as_buffer(buff) ),
                                               neo::bytewise_iterator( neo::as_buffer(rawPath) ) 
                                             )     ,
     true, "Serialization/Deserialization RawPath");
}

void
RawPathTestCase::DoRun()
{
TestRawSerialize();
TestRawDecodeFromBytes();
TestRawSerializeDecode();
TestRawReverse();
TestRawToDecoded();
TestGetInfoField();
TestGetHopField();
TestLastHop();
TestPenultimateHop();
TestSetInfoField();
TestSetHopField();

}

class RawPathTestSuite : public TestSuite
{
  public:
    RawPathTestSuite()
        : TestSuite("RawPath", UNIT)
    {
        AddTestCase(new RawPathTestCase(), TestCase::QUICK);
    }
};

static RawPathTestSuite g_pathrawTestSuite; //!< Static variable for test initialization


}
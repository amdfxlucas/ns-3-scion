#pragma once


#include "neo/byte_array.hpp"
#include "neo/bytes.hpp"

#include "ns3/decoded-path.h"
#include "ns3/hop-field.h"
#include "ns3/info-field.h"
#include "ns3/raw-path.h"
#include "ns3/scion-address.h"
#include "ns3/scion-path.h"


#include <compare>
#include <expected>


namespace ns3
{
    class SCIONTestData
    {
    public:

          class pathCase;
    DecodedPath mkDecodedPath(pathCase pcase, uint8_t i, uint8_t);
    RawPath createScionPath( uint8_t currHf, uint8_t NumHops );
    RawPath mkRawPath( pathCase pcase, uint8_t infIdx, uint8_t hopIdx );

    struct pathCase
    {
        std::vector<bool> infos;
        std::vector<std::vector<uint16_t>> hops;
    };

    struct reverseCase
    {
        pathCase input;
        pathCase want;
        std::vector<std::pair<int, int>> inIdxs;   //    [][2]int
        std::vector<std::pair<int, int>> wantIdxs; //  [][2]int
    };

    std::map<std::string, reverseCase> pathReverseCases{
        {"1 segment, 2 hops",
         reverseCase{
             .input = pathCase{{true}, {{11, 12}}},
             .want = pathCase{{false}, {{12, 11}}},
             .inIdxs = {{0, 0}, {0, 1}},
             .wantIdxs = {{0, 1}, {0, 0}},
         }},

        {"1 segment, 5 hops",
         reverseCase{
             .input = pathCase{{true}, {{11, 12, 13, 14, 15}}},
             .want = pathCase{{false}, {{15, 14, 13, 12, 11}}},
             .inIdxs = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}},
             .wantIdxs = {{0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}},
         }},

        {"2 segments, 5 hops",
         reverseCase{
             .input = pathCase{ {true, false}, { {11, 12}, {13, 14, 15} } },
             .want = pathCase{ {true, false}, { {15, 14, 13}, {12, 11} } },
             .inIdxs = { {0, 0}, {0, 1}, {1, 2}, {1, 3}, {1, 4} },
             .wantIdxs = { {1, 4}, {1, 3}, {0, 2}, {0, 1}, {0, 0} },
         }},
        {"3 segments, 9 hops",
         reverseCase{.input =
                         pathCase{
                             {true, false, false},
                             {
                                 {11, 12},
                                 {13, 14, 15, 16},
                                 {17, 18, 19},
                             },
                         },
                     .want =
                         pathCase{
                             {true, true, false},
                             {
                                 {19, 18, 17},
                                 {16, 15, 14, 13},
                                 {12, 11},
                             },
                         },
                     .inIdxs =
                         {
                             {0, 0},
                             {0, 1},
                             {1, 2},
                             {1, 3},
                             {1, 4},
                             {1, 5},
                             {2, 6},
                             {2, 7},
                             {2, 8},
                         },
                     .wantIdxs =
                         {
                             {2, 8},
                             {2, 7},
                             {1, 6},
                             {1, 5},
                             {1, 4},
                             {1, 3},
                             {0, 2},
                             {0, 1},
                             {0, 0},
                         }}}

    };

    inline static auto rawPath = neo::byte_array{
        std::byte(0x00), std::byte(0x00), std::byte(0x20), std::byte(0x80), std::byte(0x00),
        std::byte(0x00), std::byte(0x01), std::byte(0x11), std::byte(0x00), std::byte(0x00),
        std::byte(0x01), std::byte(0x00), std::byte(0x01), std::byte(0x00), std::byte(0x02),
        std::byte(0x22), std::byte(0x00), std::byte(0x00), std::byte(0x01), std::byte(0x00),
        std::byte(0x00), std::byte(0x3f), std::byte(0x00), std::byte(0x01), std::byte(0x00),
        std::byte(0x00), std::byte(0x01), std::byte(0x02), std::byte(0x03), std::byte(0x04),
        std::byte(0x05), std::byte(0x06), std::byte(0x00), std::byte(0x3f), std::byte(0x00),
        std::byte(0x03), std::byte(0x00), std::byte(0x02), std::byte(0x01), std::byte(0x02),
        std::byte(0x03), std::byte(0x04), std::byte(0x05), std::byte(0x06), std::byte(0x00),
        std::byte(0x3f), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x02),
        std::byte(0x01), std::byte(0x02), std::byte(0x03), std::byte(0x04), std::byte(0x05),
        std::byte(0x06), std::byte(0x00), std::byte(0x3f), std::byte(0x00), std::byte(0x01),
        std::byte(0x00), std::byte(0x00), std::byte(0x01), std::byte(0x02), std::byte(0x03),
        std::byte(0x04), std::byte(0x05), std::byte(0x06)};

    std::vector<InfoField> testInfoFields{InfoField{
                                              .Peer = false,
                                              .ConsDir = false,
                                              .SegID = 0x111,
                                              .Timestamp = 0x100,
                                          },
                                          InfoField{
                                              .Peer = false,
                                              .ConsDir = true,
                                              .SegID = 0x222,
                                              .Timestamp = 0x100,
                                          }};

    //	bool IngressRouterAlert;
    // bool EgressRouterAlert;
    std::vector<HopField> testHopFields{HopField{
                                            .ExpTime = 63,
                                            .ConsIngress = 1,
                                            .ConsEgress = 0,
                                            .Mac = {1, 2, 3, 4, 5, 6},
                                        },
                                        HopField{
                                            .ExpTime = 63,
                                            .ConsIngress = 3,
                                            .ConsEgress = 2,
                                            .Mac = {1, 2, 3, 4, 5, 6},
                                        },
                                        HopField{
                                            .ExpTime = 63,
                                            .ConsIngress = 0,
                                            .ConsEgress = 2,
                                            .Mac = {1, 2, 3, 4, 5, 6},
                                        },
                                        HopField{
                                            .ExpTime = 63,
                                            .ConsIngress = 1,
                                            .ConsEgress = 0,
                                            .Mac = {1, 2, 3, 4, 5, 6},
                                        }};

    MetaHdr meta{.CurrINF = 0, .CurrHF = 0, .SegLen = {2, 2, 0}};

    DecodedPath decodedTestPath{
        .base =
            Base{
                .PathMeta = meta,

                .NumINF = 2,
                .NumHops = 4,
            },
        .InfoFields = testInfoFields,
        .HopFields = testHopFields,
    };

    RawPath rawTestPath{Base{
                            .PathMeta =
                                MetaHdr{
                                    .CurrINF = 0,
                                    .CurrHF = 0,
                                    .SegLen = {2, 2, 0},
                                },
                            .NumINF = 2,
                            .NumHops = 4,
                        },
                        neo::as_buffer(rawPath)};
};


inline RawPath SCIONTestData::mkRawPath( SCIONTestData::pathCase pcase, uint8_t infIdx, uint8_t hopIdx )
{
    /*func mkRawPath(t *testing.T, pcase pathCase, infIdx, hopIdx uint8) *scion.Raw {
	t.Helper()
	decoded := mkDecodedPath(t, pcase, infIdx, hopIdx)
	raw, err := decoded.ToRaw()
	require.NoError(t, err)
	return raw
    */

   DecodedPath decoded = mkDecodedPath( pcase, infIdx,hopIdx );
   RawPath raw = decoded.ToRaw();
   return raw;
}

inline DecodedPath
SCIONTestData::mkDecodedPath(SCIONTestData::pathCase pcase,
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
}


inline RawPath SCIONTestData::createScionPath( uint8_t currHf, uint8_t NumHops )
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
    
}
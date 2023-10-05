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

#include "ns3/gopacket++.h"
#include "ns3/log.h"
#include "ns3/scion-address.h"
#include "ns3/scion-header.h"
#include "ns3/scmp-header.h"
#include "ns3/udp-layer.h"
#include "ns3/simulator.h"
#include "ns3/test.h"
#include "ns3/udp-header.h"

#include <compare>
#include <string_view>
#include "neo/as_dynamic_buffer.hpp"

#include "ns3/scmp-packet-too-big.h"

#include "scion-test-data.h"

using namespace ns3;


class SCIONHeaderTestCase : public TestCase, public SCIONTestData
{
  public:
    SCIONHeaderTestCase();
    ~SCIONHeaderTestCase() override;
    void DoRun() override;
    void Test01();
    void Test02();
    void Test03();
    void Test04();
    void TestUDP();
    void SCMPTest01();
    private:
    void CheckSCIONHeaderEqual(const SCIONHeader*, const SCIONHeader& );
    void CheckUDPLayerEqual( const UDPLayer* udplayer, const UDPLayer& udp );
};

SCIONHeaderTestCase::SCIONHeaderTestCase()
    : TestCase("Verify the Serialization and Deserialization of Packets")
{
}

SCIONHeaderTestCase::~SCIONHeaderTestCase()
{
}

void SCIONHeaderTestCase::CheckUDPLayerEqual( const UDPLayer* udplayer, const UDPLayer& udp )
{
NS_TEST_ASSERT_MSG_EQ( udplayer->GetDestinationPort(),udp.GetDestinationPort(), "wrong UDP Destination port");

NS_TEST_ASSERT_MSG_EQ( udplayer->GetSourcePort(), udp.GetSourcePort(), "wrong UDP source port ");

// TODO: Check Checksum here
}

void SCIONHeaderTestCase::CheckSCIONHeaderEqual( const SCIONHeader* scionLayer, const SCIONHeader& scion )
{
NS_TEST_ASSERT_MSG_EQ(scionLayer->GetDstIA(), scion.GetDstIA(), "d");
    NS_TEST_ASSERT_MSG_EQ(scionLayer->GetSrcIA(), scion.GetSrcIA(), "d");

    NS_TEST_ASSERT_MSG_EQ(scionLayer->GetTrafficClass(),
                          scion.GetTrafficClass(),
                          "Decoding of SCIONHeader failed (field: TrafficClass)");
    NS_TEST_ASSERT_MSG_EQ(scionLayer->GetVersion(),
                          scion.GetVersion(),
                          "Decoding of SCIONHeader failed (field: Version)");
    NS_TEST_ASSERT_MSG_EQ(scionLayer->DstAddress(),
                          scion.DstAddress(),
                          "Decoding of SCIONHeader failed (field: DstAddress)");
    NS_TEST_ASSERT_MSG_EQ(scionLayer->SrcAddress(),
                          scion.SrcAddress(),
                          "Decoding of SCIONHeader failed (field: SrcAddress)");
    NS_TEST_ASSERT_MSG_EQ(scionLayer->NextHeader(),
                          scion.NextHeader(),
                          "Decoding of SCIONHeader failed (field: NextHeader)");
    NS_TEST_ASSERT_MSG_EQ(scionLayer->GetPayloadLength(),
                          scion.GetPayloadLength(),
                          "Decoding of SCIONHeader failed (field: PayloadLen)");
    NS_ASSERT_MSG(scionLayer->GetDstAddrType() == AddrType_t::T4Ip, "AddressType wrong");
    NS_ASSERT_MSG(scionLayer->GetSrcAddrType() == AddrType_t::T16Ip, "AddressType wrong");



    NS_TEST_ASSERT_MSG_EQ(scionLayer->SCIONSrcAddress(),
                          scion.SCIONSrcAddress(),
                          "Decoding of SCIONHeader failed (field: SCIONSrcAddress)");
    NS_TEST_ASSERT_MSG_EQ(scionLayer->SCIONDstAddress(),
                          scion.SCIONDstAddress(),
                          "Decoding of SCIONHeader failed (field: SCIONDstAddress)");
}

void SCIONHeaderTestCase::SCMPTest01()
{
 auto pkt = Create<GoPacket>();

    SCMPHeader  scmp_header{SCMP::TypePacketTooBig};
    SCMPPacketTooBig scmp{1500};

    Payload_t payload;


    using namespace std::literals;
    const auto string_payload = "<payload>I am a small string payload</payload>"sv;
    payload.m_bytes = neo::bytes::copy(neo::as_buffer(string_payload));

  
    SCIONAddress a0{"19-ffaa:0:1067,127.0.0.1"};    
    SCIONAddress a1{"19-ffaa:0:1067,[2001:db8:85a3:8d3:1319:8a2e:370:7348]"};  

    SCIONHeader scion;
    scion.SetDstAddress(a0);
    scion.SetSrcAddress(a1);

    scion.SetPath( decodedTestPath );

    scion.SetNextHeader(L4ProtocolType_t::L4SCMP );
    scion.SetPayloadLength( payload.GetSerializedSize()
                           + scmp.GetSerializedSize()
                           + scmp_header.GetSerializedSize() );

    SCIONHeader scion_copy{scion};
    Payload_t payload_copy{payload};

    pkt->AddLayer( std::move(scion_copy) );
    pkt->AddLayer( std::move(scmp_header ) );
    pkt->AddLayer( std::move( scmp));
    pkt->AddLayer( std::move(payload_copy) );
    

    pkt->Serialize();

    Packet p = *static_cast<Packet*>( PeekPointer(pkt) );
// Do opject slicing here intentionally to discard the Derived GoPacket's Layers
// so that they have to be decoded again

   Ptr<GoPacket> pktcpy = Create<GoPacket>(p);


    pktcpy->set_decode_recursive(true );
    pktcpy->Decode( SCIONHeader::staticLayerType() );

    
    auto layer = pktcpy->Layers().at(0);

    NS_TEST_ASSERT_MSG_EQ(pkt->Layers().size(), 4, "Wrong number of Layers decoded" );
   //NS_LOG( "decoded: " << pktcpy->Layers().size() << " layers" );

    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(layer), true, "No layer decoded at all");

    auto scionLayer = std::dynamic_pointer_cast<SCIONHeader>(layer);
    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(scionLayer), true, "No scionlayer decoded ");

    auto pL = pktcpy->Layers().at(3);
    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(pL), true, "No layer decoded at all");
    auto payLay = std::dynamic_pointer_cast<Payload_t>(pL);
    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(payLay), true, "No payloadlayer decoded ");

    auto scmp_too_big  = pktcpy->Layers().at(2);
    NS_TEST_ASSERT_MSG_EQ( static_cast<bool>(scmp_too_big),true,"no scmp layer decoded" );
    auto scm = std::dynamic_pointer_cast<SCMPPacketTooBig>( scmp_too_big );
    NS_TEST_ASSERT_MSG_EQ( static_cast<bool>(scm), true, " no scmp packet too big layer decoded" );
    NS_TEST_ASSERT_MSG_EQ( scm->GetMtu(), scmp.GetMtu(), "wrong MTU decoded ");
          

    NS_TEST_ASSERT_MSG_EQ( std::string_view( neo::const_buffer(payLay->m_bytes)  ),string_payload ,"payload serialization failed" );
    
    CheckSCIONHeaderEqual( scionLayer.get(), scion );
}

void SCIONHeaderTestCase::Test01()
{
  auto pkt = Create<GoPacket>();

  
    SCIONAddress a0{"19-ffaa:0:1067,127.0.0.1"};
    NS_ASSERT_MSG(Ipv4Address::GetType() == a0.GetHostAddress().GetType(), "AddressType wrong");

    // SCIONAddress a1{"17-ffbb:2:6710,127.0.0.2"};
    SCIONAddress a1{"19-ffaa:0:1067,[2001:db8:85a3:8d3:1319:8a2e:370:7348]"};
    NS_ASSERT_MSG(Ipv6Address::GetType() == a1.GetHostAddress().GetType(), "AddressType wrong");

    SCIONHeader scion;
    scion.SetPath( rawTestPath );
    scion.SetDstAddress(a0);
    scion.SetSrcAddress(a1);

    NS_ASSERT_MSG(Ipv4Address::IsMatchingType(scion.DstAddress()), "AddressType wrong");
    NS_ASSERT_MSG(Ipv6Address::IsMatchingType(scion.SrcAddress()), "AddressType wrong");

    NS_ASSERT_MSG(scion.GetDstAddrType() == AddrType_t::T4Ip, "AddressType wrong");
    NS_ASSERT_MSG(scion.GetSrcAddrType() == AddrType_t::T16Ip, "AddressType wrong");

    // NS_TEST_ASSERT_MSG_EQ( scion.GetDstAddrType(), AddrType_t::T4Ip ,"AddressType wrong");
    // NS_TEST_ASSERT_MSG_EQ( scion.GetSrcAddrType(), AddrType_t::T16Ip ,"AddressType wrong");

    scion.SetNextHeader(L4ProtocolType_t::L4None);
    scion.SetPayloadLength(0);

    /*
    UDPHeader udp;
    udp.SetSourcePort( 3041);
    udp.SetDestinationPort(53);
    pkt->AddHeader(udp);
    */
    pkt->AddHeader(scion);

   auto res = pkt->InitialDecode( SCIONHeader::staticLayerType() );

   NS_TEST_ASSERT_MSG_EQ( static_cast<bool>(res), false, "unexpected error " << res.what() );

    auto layer = pkt->Layers().at(0);

    NS_TEST_ASSERT_MSG_EQ(pkt->Layers().size(), 1, "Wrong number of Layers decoded" );

    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(layer), true, "No layer decoded at all");

    auto scionLayer = std::dynamic_pointer_cast<SCIONHeader>(layer);
    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(scionLayer), true, "No scionlayer decoded ");

    //   NS_TEST_ASSERT_MSG_EQ( scionLayer->dstAddrType,scion.dstAddrType,"d");
    // NS_TEST_ASSERT_MSG_EQ( scionLayer->srcAddrType,scion.srcAddrType,"d");

   CheckSCIONHeaderEqual(scionLayer.get(), scion );
    //  Simulator::Destroy();
}

void
SCIONHeaderTestCase::DoRun()
{
  Test01();
  Test02();
  Test03();
  Test04();
  TestUDP();

  SCMPTest01();
}

void SCIONHeaderTestCase::Test02()
{
    auto pkt = Create<GoPacket>();

    Payload_t payload;


    using namespace std::literals;
    auto string_payload = "<payload>I am a small string payload!</payload>"sv;
    payload.m_bytes = neo::bytes::copy(neo::as_buffer(string_payload));

  
    SCIONAddress a0{"19-ffaa:0:1067,127.0.0.1"};    
    SCIONAddress a1{"19-ffaa:0:1067,[2001:db8:85a3:8d3:1319:8a2e:370:7348]"};  

    SCIONHeader scion;
    scion.SetPath( rawTestPath);
    scion.SetDstAddress(a0);
    scion.SetSrcAddress(a1);

    // scion.SetNextHeader(L4ProtocolType_t::L4UDP );
    scion.SetPayloadLength( payload.GetSerializedSize() );

    SCIONHeader scion_copy{scion};
    Payload_t payload_copy{payload};

    pkt->AddLayer( std::move(scion_copy) );
    pkt->AddLayer( std::move(payload_copy) );

    pkt->Serialize();

    Packet p = *static_cast<Packet*>( PeekPointer(pkt) );
// Do opject slicing here intentionally to discard the Derived GoPacket's Layers
// so that they have to be decoded again

   Ptr<GoPacket> pktcpy = Create<GoPacket>(p);



    pktcpy->Decode( SCIONHeader::staticLayerType() );

    
    auto layer = pktcpy->Layers().at(0);

    NS_TEST_ASSERT_MSG_EQ(pkt->Layers().size(), 2, "Wrong number of Layers decoded" );
   //NS_LOG( "decoded: " << pktcpy->Layers().size() << " layers" );

    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(layer), true, "No layer decoded at all");

    auto scionLayer = std::dynamic_pointer_cast<SCIONHeader>(layer);
    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(scionLayer), true, "No scionlayer decoded ");

    auto pL = pktcpy->Layers().at(1);
    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(pL), true, "No layer decoded at all");
    auto payLay = std::dynamic_pointer_cast<Payload_t>(pL);
    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(payLay), true, "No payloadlayer decoded ");

          
    NS_TEST_ASSERT_MSG_EQ( std::string_view( neo::const_buffer(payLay->m_bytes)  ),string_payload ,"payload serialization failed" );
    
  CheckSCIONHeaderEqual( scionLayer.get(), scion );



}

void SCIONHeaderTestCase::Test03()
{
    /*
    if Layers are added to the packet via GoPacket::Serialize()
    they are written to the packets buffer directly
    and no call to AddHeader() is made.
    However they can still be deserialized with Remove- or PeekHeader()
    if the PacketMetadata system is disabled, which would complain
    because it doesnt know of any Headers in the packet( since AddHeader() was never called )
    */
    PacketMetadata::Disable();
    PacketMetadata::DisableChecking();

    auto pkt = Create<GoPacket>();
    
    Payload_t payload;


    using namespace std::literals;
    const auto string_payload = "<payload>I am a small string payload</payload>"sv;
    payload.m_bytes = neo::bytes::copy(neo::as_buffer(string_payload));

  
    SCIONAddress a0{"19-ffaa:0:1067,127.0.0.1"};    
    SCIONAddress a1{"19-ffaa:0:1067,[2001:db8:85a3:8d3:1319:8a2e:370:7348]"};  

    SCIONHeader scion;
    scion.SetPath( rawTestPath );
    scion.SetDstAddress(a0);
    scion.SetSrcAddress(a1);

 //   scion.SetNextHeader(L4ProtocolType_t::L4UDP );
    scion.SetPayloadLength( payload.GetSerializedSize() );

    SCIONHeader scion_copy{scion};
    Payload_t payload_copy{payload};

    pkt->AddLayer( std::move(scion_copy) );
    pkt->AddLayer( std::move(payload_copy) );

    pkt->Serialize();

    SCIONHeader sh;
    pkt->RemoveHeader(sh);

    Payload_t pl;
    pkt->RemoveHeader(pl);

    NS_TEST_ASSERT_MSG_EQ( std::string_view( neo::const_buffer(pl.m_bytes)  ),string_payload ,"payload serialization failed" );

 
    NS_TEST_ASSERT_MSG_EQ(sh.GetDstIA(), scion.GetDstIA(), "d");
    NS_TEST_ASSERT_MSG_EQ(sh.GetSrcIA(), scion.GetSrcIA(), "d");

    NS_TEST_ASSERT_MSG_EQ(sh.GetTrafficClass(),
                          scion.GetTrafficClass(),
                          "Decoding of SCIONHeader failed (field: TrafficClass)");
    NS_TEST_ASSERT_MSG_EQ(sh.GetVersion(),
                          scion.GetVersion(),
                          "Decoding of SCIONHeader failed (field: Version)");
    NS_TEST_ASSERT_MSG_EQ(sh.DstAddress(),
                          scion.DstAddress(),
                          "Decoding of SCIONHeader failed (field: DstAddress)");
    NS_TEST_ASSERT_MSG_EQ(sh.SrcAddress(),
                          scion.SrcAddress(),
                          "Decoding of SCIONHeader failed (field: SrcAddress)");
    NS_TEST_ASSERT_MSG_EQ(sh.NextHeader(),
                          scion.NextHeader(),
                          "Decoding of SCIONHeader failed (field: NextHeader)");
    NS_TEST_ASSERT_MSG_EQ(sh.GetPayloadLength(),
                          scion.GetPayloadLength(),
                          "Decoding of SCIONHeader failed (field: PayloadLen)");
    NS_ASSERT_MSG(sh.GetDstAddrType() == AddrType_t::T4Ip, "AddressType wrong");
    NS_ASSERT_MSG(sh.GetSrcAddrType() == AddrType_t::T16Ip, "AddressType wrong");



    NS_TEST_ASSERT_MSG_EQ(sh.SCIONSrcAddress(),
                          scion.SCIONSrcAddress(),
                          "Decoding of SCIONHeader failed (field: SCIONSrcAddress)");
    NS_TEST_ASSERT_MSG_EQ(sh.SCIONDstAddress(),
                          scion.SCIONDstAddress(),
                          "Decoding of SCIONHeader failed (field: SCIONDstAddress)");


    // Packet p = *static_cast<Packet*>( PeekPointer(pkt) );
    // Do opject slicing here intentionally to discard the Derived GoPacket's Layers
    // so that they have to be decoded again

    // Ptr<GoPacket> pktcpy = Create<GoPacket>(p);


/*

    pktcpy->Decode(&LayerTypeSCpkt->Serialize();ION);

    /// auto layer = pkt->Layer( &LayerTypeSCION );
    auto layer = pktcpy->Layers().at(0);

    NS_TEST_ASSERT_MSG_EQ(pkt->Layers().size(), 2, "Wrong number of Layers decoded" );
   //NS_LOG( "decoded: " << pktcpy->Layers().size() << " layers" );

    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(layer), true, "No layer decoded at all");

    auto scionLayer = std::dynamic_pointer_cast<SCIONHeader>(layer);
    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(scionLayer), true, "No scionlayer decoded ");

    auto pL = pktcpy->Layers().at(1);
    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(pL), true, "No layer decoded at all");
    auto payLay = std::dynamic_pointer_cast<Payload_t>(pL);
    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(payLay), true, "No payloadlayer decoded ");

          
    NS_TEST_ASSERT_MSG_EQ( std::string_view( neo::const_buffer(payLay->m_bytes)  ),"I am a small string payload"sv ,"d" );
   
*/

}


void SCIONHeaderTestCase::Test04()
{
    /*
    this test does the opposite of test03
    It uses AddHeader() to write a ScionHeader with Payload to the packets buffer.
    But then decodes them as Layers instead of resorting to RemoveHeader()
    */
    auto pkt = Create<GoPacket>();

    Payload_t payload;


    using namespace std::literals;
    const auto string_payload = "<payload>I am a small string payload</payload>"sv;
    payload.m_bytes = neo::bytes::copy(neo::as_buffer(string_payload));

  
    SCIONAddress a0{"19-ffaa:0:1067,127.0.0.1"};    
    SCIONAddress a1{"19-ffaa:0:1067,[2001:db8:85a3:8d3:1319:8a2e:370:7348]"};  

    SCIONHeader scion;
    scion.SetPath( rawTestPath );
    scion.SetDstAddress(a0);
    scion.SetSrcAddress(a1);

  //  scion.SetNextHeader(L4ProtocolType_t::L4UDP );
    scion.SetPayloadLength( payload.GetSerializedSize() );

    SCIONHeader scion_copy{scion};
    Payload_t payload_copy{payload};
    
    pkt->AddHeader( payload_copy );
    pkt->AddHeader( scion_copy );
   

    
    Packet p = *static_cast<Packet*>( PeekPointer(pkt) );
// Do opject slicing here intentionally to discard the Derived GoPacket's Layers
// so that they have to be decoded again

   Ptr<GoPacket> pktcpy = Create<GoPacket>(p);



    pktcpy->Decode( SCIONHeader::staticLayerType() );


    auto layer = pktcpy->Layers().at(0);

    NS_TEST_ASSERT_MSG_EQ(pktcpy->Layers().size(), 2, "Wrong number of Layers decoded" );
   //NS_LOG( "decoded: " << pktcpy->Layers().size() << " layers" );

    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(layer), true, "No layer decoded at all");

    auto scionLayer = std::dynamic_pointer_cast<SCIONHeader>(layer);
    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(scionLayer), true, "No scionlayer decoded ");

    auto pL = pktcpy->Layers().at(1);
    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(pL), true, "No layer decoded at all");
    auto payLay = std::dynamic_pointer_cast<Payload_t>(pL);
    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(payLay), true, "No payloadlayer decoded ");

          
    NS_TEST_ASSERT_MSG_EQ( std::string_view( neo::const_buffer(payLay->m_bytes)  ),string_payload ,"d" );
    
   CheckSCIONHeaderEqual(scionLayer.get(), scion );

}

void SCIONHeaderTestCase::TestUDP()
{
    auto pkt = Create<GoPacket>();

    Payload_t payload;

    
    UDPLayer udp;
    udp.SetSourcePort( 3041);
    udp.SetDestinationPort(53);

    using namespace std::literals;
    auto string_payload = "<payload>I am a small string payload!</payload>"sv;
    payload.m_bytes = neo::bytes::copy(neo::as_buffer(string_payload));

  
    SCIONAddress a0{"19-ffaa:0:1067,127.0.0.1"};    
    SCIONAddress a1{"19-ffaa:0:1067,[2001:db8:85a3:8d3:1319:8a2e:370:7348]"};  

    SCIONHeader scion;
    scion.SetPath( rawTestPath);
    scion.SetDstAddress(a0);
    scion.SetSrcAddress(a1);

    scion.SetNextHeader(L4ProtocolType_t::L4UDP );
    scion.SetPayloadLength( payload.GetSerializedSize()+udp.GetSerializedSize() );

    SCIONHeader scion_copy{scion};
    UDPLayer udp_cpy{udp};
    Payload_t payload_copy{payload};

    pkt->AddLayer( std::move(scion_copy) );
    pkt->AddLayer( std::move(udp) );
    pkt->AddLayer( std::move(payload_copy) );

    pkt->Serialize();

    Packet p = *static_cast<Packet*>( PeekPointer(pkt) );
// Do opject slicing here intentionally to discard the Derived GoPacket's Layers
// so that they have to be decoded again

   Ptr<GoPacket> pktcpy = Create<GoPacket>(p);


    pktcpy->set_decode_recursive(true);
    pktcpy->Decode( SCIONHeader::staticLayerType() );

    
    auto layer = pktcpy->Layers().at(0);

    NS_TEST_ASSERT_MSG_EQ(pkt->Layers().size(), 3, "Wrong number of Layers decoded" ); // this is  the wrong packet !!!
    NS_TEST_ASSERT_MSG_EQ( pktcpy->Layers().size(), 3, "Wrong number of Layers decoded" );

   //NS_LOG( "decoded: " << pktcpy->Layers().size() << " layers" );

    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(layer), true, "No layer decoded at all");

    auto scionLayer = std::dynamic_pointer_cast<SCIONHeader>(layer);
    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(scionLayer), true, "No scionlayer decoded ");

    auto pL = pktcpy->Layers().at(2);
    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(pL), true, "No payload layer decoded");
    auto payLay = std::dynamic_pointer_cast<Payload_t>(pL);
    NS_TEST_ASSERT_MSG_EQ(static_cast<bool>(payLay), true, "No payloadlayer decoded ");

          
    NS_TEST_ASSERT_MSG_EQ( std::string_view( neo::const_buffer(payLay->m_bytes)  ),string_payload ,"payload serialization failed" );


    auto udpl = pktcpy->Layers().at(1);
    NS_TEST_ASSERT_MSG_EQ( static_cast<bool>(udpl),true,"no udp layer decoded");
    auto layer_udp = std::dynamic_pointer_cast<UDPLayer>(udpl);
    NS_TEST_ASSERT_MSG_EQ( static_cast<bool>(layer_udp), true, "no udp layer decoded");

    CheckUDPLayerEqual( layer_udp.get(), udp);
    
  CheckSCIONHeaderEqual( scionLayer.get(), scion );



}


class SCIONHeaderTestSuite : public TestSuite
{
  public:
    SCIONHeaderTestSuite()
        : TestSuite("SCIONHeader", UNIT)
    {
        AddTestCase(new SCIONHeaderTestCase(), TestCase::QUICK);
    }
};

static SCIONHeaderTestSuite _g_TestSuite; //!< Static variable for test initialization

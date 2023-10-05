#pragma once
#include "ns3/gopacket++.h"
#include "ns3/l4-address.h"
#include "ns3/packet.h"
#include "ns3/scion-header.h"
#include "ns3/scion-path.h"
#include "ns3/scion-types.h"

#include <stdint.h>
#include "ns3/go-errors.h"
#include "ns3/info-field.h"
#include "ns3/hop-field.h"

#include <tuple>
#include <utility>
#include "ns3/scmp-typecodes.h"

namespace ns3
{
class BorderRouterRoutingState;

struct processResult
{
    ASIFID_t EgressID;
    L4Address OutAddr;

    Ptr<Packet> OutPkt;
};

// scionPacketProcessor processes packets. It contains pre-allocated per-packet
// mutable state and context information which should be reused.
class PacketProcessor
{
  public:
    PacketProcessor(BorderRouterRoutingState* d);

  
    using result_t = std::pair<processResult, error>;

    result_t processPkt(Ptr<Packet> rawPkt, const L4Address& srcAddr, ASIFID_t ingressID);

    result_t processSCION();
    // result_t processEPIC();
    // result_t processOneHop();
  private:
    result_t process();

    result_t parsePath();
    result_t determinePeer();
    result_t validateHopExpiry();
    result_t validateIngressID();
    result_t validateEgressID();
    result_t validatePktLen();
    result_t validateTransitUnderlaySrc();
    result_t validateSrcDstIA();
    result_t updateNonConsDirIngressSegID();
    result_t verifyCurrentMAC();
    result_t handleIngressRouterAlert();
    result_t handleEgressRouterAlert();

    uint16_t egressInterface() const;
    uint16_t ingressInterface() const;

    bool& egressRouterAlertFlag();
    bool& ingressRouterAlertFlag();

    uint16_t currentInfoPointer() const;
    uint16_t currentHopPointer() const;

    result_t invalidDstIA();
    result_t invalidSrcIA();

    error processEgress();
    std::tuple<L4Address, processResult,error> resolveInbound();
    result_t doXover();

    result_t packSCMP( SCMPType, SCMPCode, std::shared_ptr<Layer> scmp_layer , error cause );
    std::pair< Ptr<Packet>, error > prepareSCMP( SCMPType , SCMPCode,  std::shared_ptr<Layer> scmp_layer, error cause);

private:
    // ingressID is the interface ID this packet came in, determined from the
    // socket.
    ASIFID_t ingressID;

    // rawPkt is the raw packet, it is updated during processing to contain the
    // message to send out.
    Ptr<GoPacket> rawPkt; // or GoPacket

    // srcAddr is the source address of the packet
    L4Address srcAddr;

    // buffer is the buffer that can be used to serialize gopacket layers.
    // buffer gopacket.SerializeBuffer
    // mac is the hasher for the MAC computation.
    // mac hash.Hash

    // scionLayer is the SCION gopacket layer.    
    const SCIONHeader* scionLayer = nullptr;
    // hbhLayer   slayers.HopByHopExtnSkipper
    // e2eLayer   slayers.EndToEndExtnSkipper

    // last is the last parsed layer, i.e. either &scionLayer, &hbhLayer or &e2eLayer
    const Layer* lastLayer = nullptr;

    // path is the raw SCION path. Will be set during processing.
    // path *scion.Raw
    RawPath* path;
    // Path path;

    // hopField is the current hopField field, is updated during processing.
    HopField hopField;

    // infoField is the current infoField field, is updated during processing.
    InfoField infoField;

    // effectiveXover indicates if a cross-over segment change was done during processing.
    bool effectiveXover;

    // peering indicates that the hop field being processed is a peering hop field.
    bool peering;

    bool IsPeering()const{ return peering; }

    // cachedMac contains the full 16 bytes of the MAC. Will be set during processing.
    // For a hop performing an Xover, it is the MAC corresponding to the down segment.
    // cachedMac []byte

    // macBuffers avoid allocating memory during processing.
    // macBuffers macBuffers

    // bfdLayer is reusable buffer for parsing BFD messages
    // bfdLayer layers.BFD

    // optAuth is a reusable Packet Authenticator Option
    // optAuth slayers.PacketAuthOption

    // validAuthBuf is a reusable buffer for the authentication tag
    // to be used in the hasValidAuth() method.
    // validAuthBuf []byte

    // DRKey key derivation for SCMP authentication
    // drkeyProvider drkeyProvider

    BorderRouterRoutingState* m_dataplane;
};
} // namespace ns3
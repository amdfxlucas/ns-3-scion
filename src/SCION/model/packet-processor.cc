#include "ns3/packet-processor.h"
#include "ns3/basic-error.h"
#include "ns3/basic-error.h"
#include "ns3/scmp-error.h"
#include "ns3/placeholder.h"
#include "ns3/scmp_parameter_problem.h"

/* these codes are only for the internal implementation of the packetProcessor */
namespace
{
    using namespace ns3;
	// const basic_error alreadySet {"already set"};

	const basic_error e_invalidSrcIA {"invalid source ISD-AS"};      // only ever used as input for packSCMP
	const basic_error  e_invalidDstIA {"invalid destination ISD-AS"}; // only ever used as input for packSCMP

	// expired Hop
	// invalid ingress interface

	const basic_error  e_invalidSrcAddrForTransit {"invalid source address for transit pkt"};
	const basic_error  e_cannotRoute              {"cannot route, dropping pkt"};
	const basic_error  e_emptyValue               {"empty value"};
	const basic_error  e_malformedPath            {"malformed path content"};

	const basic_error  e_modifyExisting {"modifying a running dataplane is not allowed"};
	const basic_error  e_noSVCBackend   {"cannot find internal IP for the SVC"};

	const basic_error  e_unsupportedPathType           {"unsupported path type"};
	const basic_error  e_unsupportedPathTypeNextHeader {"unsupported combination"};

	/*const basic_error noBFDSessionFound      {"no BFD sessions was found"};
	const basic_error  noBFDSessionConfigured {"no BFD sessions have been configured"};
	const basic_error  errBFDDisabled         {"BFD is disabled"};
	const basic_error  errBFDSessionDown      {"bfd session down"}; */

	const basic_error  e_PeeringEmptySeg0    {"zero-length segment[0] in peering path"};
	const basic_error  e_PeeringEmptySeg1    {"zero-length segment[1] in peering path"};
	const basic_error  e_PeeringNonemptySeg2 {"non-zero-length segment[2] in peering path"};

	const basic_error  e_ShortPacket {"Packet is too short"};
}

namespace ns3
{

    PacketProcessor::result_t PacketProcessor::invalidDstIA()
    {
        return packSCMP( SCMP::TypeParameterProblem,
                         SCMP::CodeInvalidDestinationAddress,
                         std::make_shared<SCMPParameterProblem>(CmnHdrLen)
        , e_invalidDstIA
        );
    }

    PacketProcessor::result_t PacketProcessor::invalidSrcIA()
    {
        return packSCMP( SCMP::TypeParameterProblem,
                        SCMP::CodeInvalidSourceAddress,
                        std::make_shared<SCMPParameterProblem>(CmnHdrLen+IABYTES ),
                        e_invalidSrcIA );
    }

    PacketProcessor::PacketProcessor(BorderRouterRoutingState *d)
        : m_dataplane(d)
    {
    }

    PacketProcessor::result_t PacketProcessor::processPkt( Ptr<Packet> pkt, const L4Address &src_addr, ASIFID_t ingress_ID)
    {
        srcAddr = src_addr;
        ingressID = ingress_ID;
        
        if(auto packet = DynamicCast<GoPacket>(pkt); packet)
        { // packet already is a GoPacket
            rawPkt = packet;
          // then no decoding is required ?!  
        }else
        {
        
        rawPkt = Create<GoPacket>( PeekPointer(pkt) );

        rawPkt ->set_decode_recursive(true);
        auto err =    rawPkt->Decode( SCIONHeader::staticLayerType() );
        if (err)
        {return { {},err }; }
        }

        scionLayer = rawPkt->Layer( SCIONHeader::staticLayerType() );
        if(!scionLayer){ return { {}, proto_error{"No SCION Header in packet"} }; }
        lastLayer = rawPkt->Layers().back().get();


        switch( scionLayer->GetPathType() )
        {

            case pathType_t::RawPath:
            {
                return processSCION();
            }
            case pathType_t::EmptyPath:            
            case pathType_t::OneHopPath:            
            case pathType_t::EpicPath:
            case pathType_t::DecodedPath:
            default:
            {
                return { { }, basic_error{e_unsupportedPathType, "type", std::to_string(pathType) } };
            }    
        };


    }

    PacketProcessor::result_t PacketProcessor::processSCION()
    {

        if(  auto rawpath = scionLayer->GetPath().As<RawPath>(); rawpath)
        {
            path = rawpath.value();
        } else
        {
            return { {}, e_malformedPath  };
        }

       return process();
    }

    PacketProcessor::result_t PacketProcessor::process()
    {
     
     if ( auto[ _,e] = parsePath(); e )
     {
        return { {},e };
     }
      if( auto[ _,e] = determinePeer(); e)
      {
        return { {}, e};
      }
     if(   auto [r ,e] = validateHopExpiry(); e)
     {
        return {r,e};
     }
      if(  auto [ r,e] = validateIngressID(); e)
      {
            return {r,e};
      }
      if(  auto [ r,e] = validatePktLen();e )
      {
        return {r,e};
      }
      if(  auto [_,e] = validateTransitUnderlaySrc(); e)
      {
        return { {},e };
      }
       if( auto [r,e] = validateSrcDstIA(); e)
       {
        return {r,e};
       }
       if( auto e = updateNonConsDirIngressSegID(); e)
       {
            return {{},e};
       }
       if( auto[ r,e] = verifyCurrentMAC(); e)
       {
        return {r,e};
       }
       if( auto[ r,e] = handleIngressRouterAlert();e)
       {
        return {r,e};
       }

        // inbound: pkts destined to the local IA
       if(scionLayer->GetDstIA() == m_dataplane->GetLocalIA() )
       {
            if( =resolveInbound(); )
            {

            }else
            {
                return { processResult{.OutAddr = a,.OutPkt =rawPkt } ,{ } } ;
            }

       }

	// Outbound: pkts leaving the local IA.
	// BRTransit: pkts leaving from the same BR different interface.
       if( path.IsXover() && !IsPeering() )
       {

		// An effective cross-over is a change of segment other than at
		// a peering hop.
        if(  auto[_,e] = doXover(); e )
        {
            return { {},e };
        }

	    // doXover() has changed the current segment and hop field.
		// We need to validate the new hop field.
        if( auto [r,e] = validateHopExpiry(); e)
        {
            return {r, basic_error{ e,"info", "after Xover"} };
        }

    		// verify the new block
        if( auto [r,e] = verifyCurrentMAC();e )
        {
            return {r, basic_error{e, "info", "after Xover"} };
        }

       }

    if( auto [r,e] = validateEgressID(); e)
    {
        return {r,e};
    }

	// handle egress router alert before we check if it's up because we want to
	// send the reply anyway, so that trace route can pinpoint the exact link
	// that failed.
    if( auto [r,e] = handleEgressRouterAlert(); e )
    {
        return {r,e};
    }

    if( auto [r,e] = validateEgressUp() ; e )
    {
        return {r,e};
    }



    auto egress_id = egressInterface();

    /*
    	if _, ok := p.d.external[egress_id]; ok {
		if err := p.processEgress(); err != nil {
			return processResult{}, err
		}
		return processResult{ .EgressID = egress_id, .OutPkt= rawPkt}, {}
        }

	// ASTransit: pkts leaving from another AS BR.
	if a, ok := p.d.internalNextHops[egressID]; ok {
		return processResult{ .OutAddr = a, .OutPkt= rawPkt}, nil
	}
    */

    auto error_code = info_field.ConsDir ? SCMP::CodeUnknownHopFieldEgress
                                        : SCMP::CodeUnknownHopFieldIngress;

    return packSCMP( SCMP::TypeParameterProblem,
                    error_code,
                    SCMPParameterProblem{ currentHopPointer() },
                    e_cannotRoute );
        
    }

    PacketProcessor::result_t PacketProcessor::parsePath()
    {
        hopField = path.GetCurrentHopField();

        infoField = path.GetCurrentInfoField();
        return {};
    }
    
    PacketProcessor::result_t PacketProcessor::determinePeer()
    {
        if (!infoField.Peer)
        {
            return {};
        }

        if (path.PathMeta.SegLen[0] == 0)
        {
            throw std::runtime_error("zero-length segment[0] in peering path");
        }
        if (path.PathMeta.SegLen[1] == 0)
        {
            throw std::runtime_error("zero-length segment[1] in peering path");
        }
        if (path.PathMeta.SegLen[2] != 0)
        {
            throw std::runtime_error("non-zero-length segment[2] in peering path");
        }
        // The peer hop fields are the last hop field on the first path
        // segment (at SegLen[0] - 1) and the first hop field of the second
        // path segment (at SegLen[0]). The below check applies only
        // because we already know this is a well-formed peering path.
        auto currHF = path.PathMeta.CurrHF;
        auto segLen = path.PathMeta.SegLen[0];
        peering = (currHF == segLen - 1) || (currHF == segLen);
        return processResult{};
    }

    PacketProcessor::result_t PacketProcessor::validateHopExpiry()
    {
    }

    PacketProcessor::result_t PacketProcessor::validateIngressID()
    {
        auto pktIngressID = hopField.ConsIngress;

        if (!infoField.ConsDir)
        {
            pktIngressID = hopField.ConsEgress;
        }

        if (ingressID != 0 && ingressID != pktIngressID)
        {
            // return scmp Stuff
            NS_ASSERT_MSG(false, "ingress interface invalid");
        }

        return {};
    }
    
    PacketProcessor::result_t PacketProcessor::validateEgressID()
    {
        auto pktEgressID = egressInterface();

        auto ih = internalNextHops.contains( pktEgressID),
        auto eh = external.contains( pktEgressID );
        if( !ih && ! eh )
        {
            SCMPCode errCode = infoField.ConsDir ? SCMP::CodeUnknownHopFieldEgress :
                                                    SCMP::CodeUnknownHopFieldIngress;
            
            // return  scmp error

        }

        auto ingress = m_dataplane ->linkTypes.at(ingressID);
        auto egress = m_dataplane ->linkTypes.at( pktEgressID);
        if(!effectiveXover)
        {
        // Check that the interface pair is valid within a single segment.
		// No check required if the packet is received from an internal interface.
		// This case applies to peering hops as a peering hop isn't an effective
		// cross-over (eventhough it is a segment change).



        }
    }

    PacketProcessor::result_t PacketProcessor::validatePktLen()
    {
        //if( scionLayer.PayloadLen == len(scionLayer.Payload) )
        {
            return {};
        }

        return packSCMP( SCMP::TypeParameterProblem,
                        SCMP::CodeInvalidPacketSize,
                        SCMPParameterProblem{.Pointer=0},
                        basic_error{"bad packet size", 
                        "header", 
                        "actual", 
                        } 
                        );
    }

    uint16_t PacketProcessor::currentInfoPointer() const
    {
return CmnHdrLen + scionLayer.AddrHdrLen() + METALEN + INFO_FIELD_LEN* path.PathMeta.CurrINF;
    }

        uint16_t PacketProcessor::currentHopPointer() const
        {
return CmnHdrLen + scionLayer.AddrHdrLen() + METALEN + INFO_FIELD_LEN * path.PathMeta.CurrINF;
        }

    // validateTransitUnderlaySrc checks that the source address of transit packets
    // matches the expected sibling router.
    // Provided that underlying network infrastructure prevents address spoofing,
    // this check prevents malicious end hosts in the local AS from bypassing the
    // SrcIA checks by disguising packets as transit traffic.
    PacketProcessor::result_t PacketProcessor::validateTransitUnderlaySrc()
    {
        if (path.IsFirstHop() || ingressID != 0)
        {
            // not a transit packet, nothing to check
            return {};
        }

        auto pktIngressID = ingressInterface();
        auto hasNextHop = internalNextHops.contains(pktIngressID);
        auto expectedSrc = internalNextHops.at(pktIngressID);
        if (!hasNextHop || (expectedSrc != srcAddr))
        {
            // drop
            throw std::runtime_error("invalid source address for transit pkt");
        }
        return {};
    }

    PacketProcessor::result_t PacketProcessor::validateSrcDstIA()
    {
        auto srcIsLocal =
            auto dstIsLocal =

        if (ingressID == 0) // this packet was received over the internalInterface (with id==0)
        {
            // Outbound
            // Only check SrcIA if first hop, for transit this already checked by ingress router.
            // Note: SCMP error messages triggered by the sibling router may use paths that
            // don't start with the first hop.
            if (path.IsFirstHop() && !srcIsLocal)
            {
                // return SCMP invalid srcIA
                NS_ASSERT_MSG(false, "invalid srcIA");
            }
            if (dstIsLocal)
            {
                // return SCMP invalid dstIA
                NS_ASSERT_MSG(false, "invalid dstIA");
            }
        }
        else
        {
            // inbound
            if (srcIsLocal)
            { // return SCMP invalid srcIA
                NS_ASSERT_MSG(false, "invalid srcIA");
            }
            if (path.IsLastHop() != dstIsLocal)
            {
                // return SCMP invalid dstIA
                NS_ASSERT_MSG(false, "invalid dstIA");
            }
        }
        return {};
    }

    PacketProcessor::result_t PacketProcessor::updateNonConsDirIngressSegID()
    {
    }

    PacketProcessor::result_t PacketProcessor::verifyCurrentMAC()
    {
    }

    PacketProcessor::result_t PacketProcessor::handleIngressRouterAlert()
    {
    }

    PacketProcessor::result_t PacketProcessor::handleEgressRouterAlert()
    {
    }

    uint16_t PacketProcessor::ingressInterface() const
    {
        InfoField info = infoField;
        HopField hop = hopField;

        if (!peering && path.IsFirstHopAfterXover())
        {
            info = path.GetInfoField(path.PathMeta.CuffINF - 1);
            hop = path.GetHopField(path.PathMeta.CurrHF - 1);
        }

        if (info.ConsDir)
        {
            return hop.ConsIngress;
        }
        return hop.ConsEgress;
    }

    uint16_t PacketProcessor::egressInterface() const
    {
        if (infoField.ConsDir)
        {
            return hopField.ConsIngress;
        }
        return hopField.ConsIngress;
    }

    error PacketProcessor::processEgress()
    {
        // We are the egress router and if we go in construction direction we
        // need to update the SegID (unless we are effecting a peering hop).
        // When we're at a peering hop, the SegID for this hop and for the next
        // are one and the same, both hops chain to the same parent. So do not
        // update SegID.

        if (infoField.ConsDir && !peering)
        {
            infoField.UpdateSegID(hopField.Mac);

            path.SetInfoField(infoField, path.PathMeta.CuffINF);
        }
        path.IncPath();
    }

    bool& PacketProcessor::egressRouterAlertFlag()
    {
        if(! infoField.ConsDir )
        {
            return hopField.IngressRouterAlert;
        }
        return hopField.EgressRouterAlert;
    }

        bool& PacketProcessor::ingressRouterAlertFlag()
        {
            if(!infoField.ConsDir )
            {
                return hopField.EgressRouterAlert;
            }
            return hopField.IngressRouterAlert;
        }

    std::tuple<L4Address, processResult,error> PacketProcessor::resolveInbound()
    {

    }

    PacketProcessor::result_t PacketProcessor::doXover()
    {
        effectiveXover = true;
        path.IncPath();
        hopField = path.GetCurrentHopField();
        infoField = path.GetCurrentInfoField();
        return {};
    }

}
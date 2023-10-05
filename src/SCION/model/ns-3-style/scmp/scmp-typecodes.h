#pragma once
#include <stdint.h>
#include <string>
#include <map>
#include "ns3/endian-utils.h"

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


// SCMPType is the type of the SCMP type code.
using SCMPType = uint8_t;

// SCMPCode is the code of the SCMP type code.
using SCMPCode = uint8_t;

namespace SCMP
{
// SCMP error messages.
enum : uint8_t
{
	TypeDestinationUnreachable   = 1,
	TypePacketTooBig              = 2,
	TypeParameterProblem          = 4,
	TypeExternalInterfaceDown     = 5,
	TypeInternalConnectivityDown  = 6
};

// Destination unreachable codes
enum : uint8_t 
{
	CodeNoRoute                    = 0,
	CodeAdminDeny                  = 1,
	CodeBeyondScopeOfSourceAddr    = 2,
	CodeAddressUnreachable         = 3,
	CodePortUnreachable            = 4,
	CodeSourceAddressFailedPolicy  = 5,
	CodeRejectRouteToDest          = 6,
};

// ParameterProblem
enum : uint8_t
{
	CodeErroneousHeaderField  = 0,
	CodeUnknownNextHdrType    = 1,

	CodeInvalidCommonHeader  = 16,
	CodeUnknownSCIONVersion   = 17,
	CodeFlowIDRequired        = 18,
	CodeInvalidPacketSize     = 19,
	CodeUnknownPathType       = 20,
	CodeUnknownAddressFormat  = 21,

	CodeInvalidAddressHeader      = 32,
	CodeInvalidSourceAddress       = 33,
	CodeInvalidDestinationAddress  = 34,
	CodeNonLocalDelivery           = 35,

	CodeInvalidPath            = 48,
	CodeUnknownHopFieldIngress  = 49,
	CodeUnknownHopFieldEgress   = 50,
	CodeInvalidHopFieldMAC      = 51,
	CodePathExpired             = 52,
	CodeInvalidSegmentChange    = 53,

	CodeInvalidExtensionHeader  = 64,
	CodeUnknownHopByHopOption   = 65,
	CodeUnknownEndToEndOption   = 66,
};

//  informational messages.
enum : uint8_t
{
	TypeEchoRequest        = 128,
	TypeEchoReply          = 129,
	TypeTracerouteRequest  = 130,
	TypeTracerouteReply    = 131,
};

}

// SCMPTypeCode represents SCMP type/code case.
struct SCMPTypeCode
{
	constexpr SCMPTypeCode(){}
	constexpr SCMPTypeCode( SCMPType type, SCMPCode code =0): code(BigEndian::Uint16(type,code) )
	{}
	// operator uint16_t(){return TypeCode(); }
	uint16_t TypeCode()const {return code;}

// Type returns the SCMP type field.
constexpr SCMPType Type() const { return SCMPType( code >> 8); }
// Code returns the SCMP code field.
constexpr SCMPCode Code() const { return code; }
// InfoMsg indicates if the SCMP message is an SCMP informational message.
constexpr bool InfoMsg() const { return Type() > 127; }

inline std::string String() const;
private:
uint16_t code =0;


struct _internal_map_t{  std::string name;  std::map<SCMPCode,std::string>  codes; } ;
inline static const std::map< const SCMPType,const _internal_map_t > scmpTypeCodeInfo = {
	{ SCMP::TypeDestinationUnreachable ,   { .name= "DestinationUnreachable"} },
	{ SCMP::TypeExternalInterfaceDown ,    { .name= "ExternalInterfaceDown"} },
	{ SCMP::TypeInternalConnectivityDown, {.name= "InternalConnectivityDown"} },
	{ SCMP::TypePacketTooBig,             {.name= "PacketTooBig"} },
	{ SCMP::TypeEchoRequest,              {.name= "EchoRequest"} },
	{ SCMP::TypeEchoReply,                {.name= "EchoReply"} },
	{ SCMP::TypeTracerouteRequest,        {.name= "TracerouteRequest"} },
	{ SCMP::TypeTracerouteReply,          {.name= "TracerouteReply"} },
	{ SCMP::TypeParameterProblem,     
        { .name="ParameterProblem", .codes = {
			{ SCMP::CodeErroneousHeaderField,      "ErroneousHeaderField" },
			{ SCMP::CodeUnknownNextHdrType,        "UnknownNextHdrType" },
			{ SCMP::CodeInvalidCommonHeader,       "InvalidCommonHeader" },
			{ SCMP::CodeUnknownSCIONVersion,       "UnknownSCIONVersion" },
			{ SCMP::CodeFlowIDRequired,            "FlowIDRequired" },
			{ SCMP::CodeInvalidPacketSize,         "InvalidPacketSize" },
			{ SCMP::CodeUnknownPathType,           "UnknownPathType" },
			{ SCMP::CodeUnknownAddressFormat,      "UnknownAddressFormat" },
			{ SCMP::CodeInvalidAddressHeader,      "InvalidAddressHeader" },
			{ SCMP::CodeInvalidSourceAddress,      "InvalidSourceAddress" },
			{ SCMP::CodeInvalidDestinationAddress, "InvalidDestinationAddress" },
			{ SCMP::CodeNonLocalDelivery,          "NonLocalDelivery" },
			{ SCMP::CodeInvalidPath,               "InvalidPath" },
			{ SCMP::CodeUnknownHopFieldIngress,    "UnknownHopFieldIngressInterface" },
			{ SCMP::CodeUnknownHopFieldEgress,     "UnknownHopFieldEgressInterface" },
			{ SCMP::CodeInvalidHopFieldMAC,        "InvalidHopFieldMAC" },
			{ SCMP::CodePathExpired,               "PathExpired" },
			{ SCMP::CodeInvalidSegmentChange,      "InvalidSegmentChange" },
			{ SCMP::CodeInvalidExtensionHeader,    "InvalidExtensionHeader" },
			{ SCMP::CodeUnknownHopByHopOption,     "UnknownHopByHopOption" },
			{ SCMP::CodeUnknownEndToEndOption,     "UnknownEndToEndOption" }
		                                    }
	                                 }
    }
};

};



inline std::string SCMPTypeCode::String()const
{


	if( !scmpTypeCodeInfo.contains(Type() ) )
	{
		return std::to_string(Type()) + "(" + std::to_string(Code()) +")";
	}

	const auto& info = scmpTypeCodeInfo.at(Type());
	
	if( info.codes.size() == 0 && Code() == 0 )
	{
		return info.name;
	}

	if( !info.codes.contains(Code()) )
	{
		return info.name +"(Code: " + std::to_string( Code() ) + ")";
	}

	auto  code = info.codes.at(Code());

	return info.name +"(" + code +")";
}

}

/*
// SerializeTo writes the SCMPTypeCode value to the buffer.
func (a SCMPTypeCode) SerializeTo(bytes []byte) {
	binary.BigEndian.PutUint16(bytes, uint16(a))
}
*/


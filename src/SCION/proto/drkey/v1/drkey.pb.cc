// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: proto/drkey/v1/drkey.proto
//#include "protobuf/endian.h"
#include "proto/drkey/v1/drkey.pb.h"

#include <algorithm>
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/extension_set.h"
#include "google/protobuf/wire_format_lite.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/reflection_ops.h"
#include "google/protobuf/wire_format.h"
#include "google/protobuf/generated_message_tctable_impl.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"
PROTOBUF_PRAGMA_INIT_SEG
namespace _pb = ::google::protobuf;
namespace _pbi = ::google::protobuf::internal;
namespace _fl = ::google::protobuf::internal::field_layout;
namespace proto {
namespace drkey {
namespace v1 {
}  // namespace v1
}  // namespace drkey
}  // namespace proto
static const ::_pb::EnumDescriptor* file_level_enum_descriptors_proto_2fdrkey_2fv1_2fdrkey_2eproto[1];
static constexpr const ::_pb::ServiceDescriptor**
    file_level_service_descriptors_proto_2fdrkey_2fv1_2fdrkey_2eproto = nullptr;
const ::uint32_t TableStruct_proto_2fdrkey_2fv1_2fdrkey_2eproto::offsets[1] = {};
static constexpr ::_pbi::MigrationSchema* schemas = nullptr;
static constexpr ::_pb::Message* const* file_default_instances = nullptr;
const char descriptor_table_protodef_proto_2fdrkey_2fv1_2fdrkey_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
    "\n\032proto/drkey/v1/drkey.proto\022\016proto.drke"
    "y.v1*K\n\010Protocol\022 \n\034PROTOCOL_GENERIC_UNS"
    "PECIFIED\020\000\022\021\n\rPROTOCOL_SCMP\020\001\"\n\010\200\200\004\020\377\377\377\377"
    "\007B-Z+github.com/scionproto/scion/pkg/pro"
    "to/drkeyb\006proto3"
};
static ::absl::once_flag descriptor_table_proto_2fdrkey_2fv1_2fdrkey_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_proto_2fdrkey_2fv1_2fdrkey_2eproto = {
    false,
    false,
    176,
    descriptor_table_protodef_proto_2fdrkey_2fv1_2fdrkey_2eproto,
    "proto/drkey/v1/drkey.proto",
    &descriptor_table_proto_2fdrkey_2fv1_2fdrkey_2eproto_once,
    nullptr,
    0,
    0,
    schemas,
    file_default_instances,
    TableStruct_proto_2fdrkey_2fv1_2fdrkey_2eproto::offsets,
    nullptr,
    file_level_enum_descriptors_proto_2fdrkey_2fv1_2fdrkey_2eproto,
    file_level_service_descriptors_proto_2fdrkey_2fv1_2fdrkey_2eproto,
};

// This function exists to be marked as weak.
// It can significantly speed up compilation by breaking up LLVM's SCC
// in the .pb.cc translation units. Large translation units see a
// reduction of more than 35% of walltime for optimized builds. Without
// the weak attribute all the messages in the file, including all the
// vtables and everything they use become part of the same SCC through
// a cycle like:
// GetMetadata -> descriptor table -> default instances ->
//   vtables -> GetMetadata
// By adding a weak function here we break the connection from the
// individual vtables back into the descriptor table.
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_proto_2fdrkey_2fv1_2fdrkey_2eproto_getter() {
  return &descriptor_table_proto_2fdrkey_2fv1_2fdrkey_2eproto;
}
// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2
static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_proto_2fdrkey_2fv1_2fdrkey_2eproto(&descriptor_table_proto_2fdrkey_2fv1_2fdrkey_2eproto);
namespace proto {
namespace drkey {
namespace v1 {
const ::google::protobuf::EnumDescriptor* Protocol_descriptor() {
  ::google::protobuf::internal::AssignDescriptors(&descriptor_table_proto_2fdrkey_2fv1_2fdrkey_2eproto);
  return file_level_enum_descriptors_proto_2fdrkey_2fv1_2fdrkey_2eproto[0];
}
PROTOBUF_CONSTINIT const uint32_t Protocol_internal_data_[] = {
    131072u, 0u, };
bool Protocol_IsValid(int value) {
  return 0 <= value && value <= 1;
}
// @@protoc_insertion_point(namespace_scope)
}  // namespace v1
}  // namespace drkey
}  // namespace proto
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google
// @@protoc_insertion_point(global_scope)
#include "google/protobuf/port_undef.inc"
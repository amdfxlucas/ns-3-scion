// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: proto/gateway/v1/prefix.proto

#include "proto/gateway/v1/prefix.pb.h"

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
namespace gateway {
namespace v1 {
      template <typename>
PROTOBUF_CONSTEXPR PrefixesRequest::PrefixesRequest(::_pbi::ConstantInitialized) {}
struct PrefixesRequestDefaultTypeInternal {
  PROTOBUF_CONSTEXPR PrefixesRequestDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~PrefixesRequestDefaultTypeInternal() {}
  union {
    PrefixesRequest _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 PrefixesRequestDefaultTypeInternal _PrefixesRequest_default_instance_;

inline constexpr Prefix::Impl_::Impl_(
    ::_pbi::ConstantInitialized) noexcept
      : prefix_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()),
        mask_{0u},
        _cached_size_{0} {}

template <typename>
PROTOBUF_CONSTEXPR Prefix::Prefix(::_pbi::ConstantInitialized)
    : _impl_(::_pbi::ConstantInitialized()) {}
struct PrefixDefaultTypeInternal {
  PROTOBUF_CONSTEXPR PrefixDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~PrefixDefaultTypeInternal() {}
  union {
    Prefix _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 PrefixDefaultTypeInternal _Prefix_default_instance_;

inline constexpr PrefixesResponse::Impl_::Impl_(
    ::_pbi::ConstantInitialized) noexcept
      : prefixes_{},
        _cached_size_{0} {}

template <typename>
PROTOBUF_CONSTEXPR PrefixesResponse::PrefixesResponse(::_pbi::ConstantInitialized)
    : _impl_(::_pbi::ConstantInitialized()) {}
struct PrefixesResponseDefaultTypeInternal {
  PROTOBUF_CONSTEXPR PrefixesResponseDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~PrefixesResponseDefaultTypeInternal() {}
  union {
    PrefixesResponse _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 PrefixesResponseDefaultTypeInternal _PrefixesResponse_default_instance_;
}  // namespace v1
}  // namespace gateway
}  // namespace proto
static ::_pb::Metadata file_level_metadata_proto_2fgateway_2fv1_2fprefix_2eproto[3];
static constexpr const ::_pb::EnumDescriptor**
    file_level_enum_descriptors_proto_2fgateway_2fv1_2fprefix_2eproto = nullptr;
static constexpr const ::_pb::ServiceDescriptor**
    file_level_service_descriptors_proto_2fgateway_2fv1_2fprefix_2eproto = nullptr;
const ::uint32_t TableStruct_proto_2fgateway_2fv1_2fprefix_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(
    protodesc_cold) = {
    ~0u,  // no _has_bits_
    PROTOBUF_FIELD_OFFSET(::proto::gateway::v1::PrefixesRequest, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    ~0u,  // no _inlined_string_donated_
    ~0u,  // no _split_
    ~0u,  // no sizeof(Split)
    ~0u,  // no _has_bits_
    PROTOBUF_FIELD_OFFSET(::proto::gateway::v1::PrefixesResponse, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    ~0u,  // no _inlined_string_donated_
    ~0u,  // no _split_
    ~0u,  // no sizeof(Split)
    PROTOBUF_FIELD_OFFSET(::proto::gateway::v1::PrefixesResponse, _impl_.prefixes_),
    ~0u,  // no _has_bits_
    PROTOBUF_FIELD_OFFSET(::proto::gateway::v1::Prefix, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    ~0u,  // no _inlined_string_donated_
    ~0u,  // no _split_
    ~0u,  // no sizeof(Split)
    PROTOBUF_FIELD_OFFSET(::proto::gateway::v1::Prefix, _impl_.prefix_),
    PROTOBUF_FIELD_OFFSET(::proto::gateway::v1::Prefix, _impl_.mask_),
};

static const ::_pbi::MigrationSchema
    schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
        {0, -1, -1, sizeof(::proto::gateway::v1::PrefixesRequest)},
        {8, -1, -1, sizeof(::proto::gateway::v1::PrefixesResponse)},
        {17, -1, -1, sizeof(::proto::gateway::v1::Prefix)},
};

static const ::_pb::Message* const file_default_instances[] = {
    &::proto::gateway::v1::_PrefixesRequest_default_instance_._instance,
    &::proto::gateway::v1::_PrefixesResponse_default_instance_._instance,
    &::proto::gateway::v1::_Prefix_default_instance_._instance,
};
const char descriptor_table_protodef_proto_2fgateway_2fv1_2fprefix_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
    "\n\035proto/gateway/v1/prefix.proto\022\020proto.g"
    "ateway.v1\"\021\n\017PrefixesRequest\">\n\020Prefixes"
    "Response\022*\n\010prefixes\030\001 \003(\0132\030.proto.gatew"
    "ay.v1.Prefix\"&\n\006Prefix\022\016\n\006prefix\030\001 \001(\014\022\014"
    "\n\004mask\030\002 \001(\r2h\n\021IPPrefixesService\022S\n\010Pre"
    "fixes\022!.proto.gateway.v1.PrefixesRequest"
    "\032\".proto.gateway.v1.PrefixesResponse\"\000B/"
    "Z-github.com/scionproto/scion/pkg/proto/"
    "gatewayb\006proto3"
};
static ::absl::once_flag descriptor_table_proto_2fgateway_2fv1_2fprefix_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_proto_2fgateway_2fv1_2fprefix_2eproto = {
    false,
    false,
    335,
    descriptor_table_protodef_proto_2fgateway_2fv1_2fprefix_2eproto,
    "proto/gateway/v1/prefix.proto",
    &descriptor_table_proto_2fgateway_2fv1_2fprefix_2eproto_once,
    nullptr,
    0,
    3,
    schemas,
    file_default_instances,
    TableStruct_proto_2fgateway_2fv1_2fprefix_2eproto::offsets,
    file_level_metadata_proto_2fgateway_2fv1_2fprefix_2eproto,
    file_level_enum_descriptors_proto_2fgateway_2fv1_2fprefix_2eproto,
    file_level_service_descriptors_proto_2fgateway_2fv1_2fprefix_2eproto,
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
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_proto_2fgateway_2fv1_2fprefix_2eproto_getter() {
  return &descriptor_table_proto_2fgateway_2fv1_2fprefix_2eproto;
}
// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2
static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_proto_2fgateway_2fv1_2fprefix_2eproto(&descriptor_table_proto_2fgateway_2fv1_2fprefix_2eproto);
namespace proto {
namespace gateway {
namespace v1 {
// ===================================================================

class PrefixesRequest::_Internal {
 public:
};

PrefixesRequest::PrefixesRequest(::google::protobuf::Arena* arena)
    : ::google::protobuf::internal::ZeroFieldsBase(arena) {
  // @@protoc_insertion_point(arena_constructor:proto.gateway.v1.PrefixesRequest)
}
PrefixesRequest::PrefixesRequest(
    ::google::protobuf::Arena* arena,
    const PrefixesRequest& from)
    : ::google::protobuf::internal::ZeroFieldsBase(arena) {
  PrefixesRequest* const _this = this;
  (void)_this;
  _internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(
      from._internal_metadata_);

  // @@protoc_insertion_point(copy_constructor:proto.gateway.v1.PrefixesRequest)
}









::google::protobuf::Metadata PrefixesRequest::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_proto_2fgateway_2fv1_2fprefix_2eproto_getter, &descriptor_table_proto_2fgateway_2fv1_2fprefix_2eproto_once,
      file_level_metadata_proto_2fgateway_2fv1_2fprefix_2eproto[0]);
}
// ===================================================================

class PrefixesResponse::_Internal {
 public:
};

PrefixesResponse::PrefixesResponse(::google::protobuf::Arena* arena)
    : ::google::protobuf::Message(arena) {
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:proto.gateway.v1.PrefixesResponse)
}
inline PROTOBUF_NDEBUG_INLINE PrefixesResponse::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility, ::google::protobuf::Arena* arena,
    const Impl_& from)
      : prefixes_{visibility, arena, from.prefixes_},
        _cached_size_{0} {}

PrefixesResponse::PrefixesResponse(
    ::google::protobuf::Arena* arena,
    const PrefixesResponse& from)
    : ::google::protobuf::Message(arena) {
  PrefixesResponse* const _this = this;
  (void)_this;
  _internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(
      from._internal_metadata_);
  new (&_impl_) Impl_(internal_visibility(), arena, from._impl_);

  // @@protoc_insertion_point(copy_constructor:proto.gateway.v1.PrefixesResponse)
}
inline PROTOBUF_NDEBUG_INLINE PrefixesResponse::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility,
    ::google::protobuf::Arena* arena)
      : prefixes_{visibility, arena},
        _cached_size_{0} {}

inline void PrefixesResponse::SharedCtor(::_pb::Arena* arena) {
  new (&_impl_) Impl_(internal_visibility(), arena);
}
PrefixesResponse::~PrefixesResponse() {
  // @@protoc_insertion_point(destructor:proto.gateway.v1.PrefixesResponse)
  _internal_metadata_.Delete<::google::protobuf::UnknownFieldSet>();
  SharedDtor();
}
inline void PrefixesResponse::SharedDtor() {
  ABSL_DCHECK(GetArena() == nullptr);
  _impl_.~Impl_();
}

PROTOBUF_NOINLINE void PrefixesResponse::Clear() {
// @@protoc_insertion_point(message_clear_start:proto.gateway.v1.PrefixesResponse)
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.prefixes_.Clear();
  _internal_metadata_.Clear<::google::protobuf::UnknownFieldSet>();
}

const char* PrefixesResponse::_InternalParse(
    const char* ptr, ::_pbi::ParseContext* ctx) {
  ptr = ::_pbi::TcParser::ParseLoop(this, ptr, ctx, &_table_.header);
  return ptr;
}


PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::_pbi::TcParseTable<0, 1, 1, 0, 2> PrefixesResponse::_table_ = {
  {
    0,  // no _has_bits_
    0, // no _extensions_
    1, 0,  // max_field_number, fast_idx_mask
    offsetof(decltype(_table_), field_lookup_table),
    4294967294,  // skipmap
    offsetof(decltype(_table_), field_entries),
    1,  // num_field_entries
    1,  // num_aux_entries
    offsetof(decltype(_table_), aux_entries),
    &_PrefixesResponse_default_instance_._instance,
    ::_pbi::TcParser::GenericFallback,  // fallback
  }, {{
    // repeated .proto.gateway.v1.Prefix prefixes = 1;
    {::_pbi::TcParser::FastMtR1,
     {10, 63, 0, PROTOBUF_FIELD_OFFSET(PrefixesResponse, _impl_.prefixes_)}},
  }}, {{
    65535, 65535
  }}, {{
    // repeated .proto.gateway.v1.Prefix prefixes = 1;
    {PROTOBUF_FIELD_OFFSET(PrefixesResponse, _impl_.prefixes_), 0, 0,
    (0 | ::_fl::kFcRepeated | ::_fl::kMessage | ::_fl::kTvTable)},
  }}, {{
    {::_pbi::TcParser::GetTable<::proto::gateway::v1::Prefix>()},
  }}, {{
  }},
};

::uint8_t* PrefixesResponse::_InternalSerialize(
    ::uint8_t* target,
    ::google::protobuf::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:proto.gateway.v1.PrefixesResponse)
  ::uint32_t cached_has_bits = 0;
  (void)cached_has_bits;

  // repeated .proto.gateway.v1.Prefix prefixes = 1;
  for (unsigned i = 0,
      n = static_cast<unsigned>(this->_internal_prefixes_size()); i < n; i++) {
    const auto& repfield = this->_internal_prefixes().Get(i);
    target = ::google::protobuf::internal::WireFormatLite::
        InternalWriteMessage(1, repfield, repfield.GetCachedSize(), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target =
        ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
            _internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:proto.gateway.v1.PrefixesResponse)
  return target;
}

::size_t PrefixesResponse::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:proto.gateway.v1.PrefixesResponse)
  ::size_t total_size = 0;

  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated .proto.gateway.v1.Prefix prefixes = 1;
  total_size += 1UL * this->_internal_prefixes_size();
  for (const auto& msg : this->_internal_prefixes()) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSize(msg);
  }
  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::google::protobuf::Message::ClassData PrefixesResponse::_class_data_ = {
    PrefixesResponse::MergeImpl,
    nullptr,  // OnDemandRegisterArenaDtor
};
const ::google::protobuf::Message::ClassData* PrefixesResponse::GetClassData() const {
  return &_class_data_;
}

void PrefixesResponse::MergeImpl(::google::protobuf::Message& to_msg, const ::google::protobuf::Message& from_msg) {
  auto* const _this = static_cast<PrefixesResponse*>(&to_msg);
  auto& from = static_cast<const PrefixesResponse&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:proto.gateway.v1.PrefixesResponse)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  _this->_internal_mutable_prefixes()->MergeFrom(
      from._internal_prefixes());
  _this->_internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(from._internal_metadata_);
}

void PrefixesResponse::CopyFrom(const PrefixesResponse& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:proto.gateway.v1.PrefixesResponse)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

PROTOBUF_NOINLINE bool PrefixesResponse::IsInitialized() const {
  return true;
}

::_pbi::CachedSize* PrefixesResponse::AccessCachedSize() const {
  return &_impl_._cached_size_;
}
void PrefixesResponse::InternalSwap(PrefixesResponse* PROTOBUF_RESTRICT other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  _impl_.prefixes_.InternalSwap(&other->_impl_.prefixes_);
}

::google::protobuf::Metadata PrefixesResponse::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_proto_2fgateway_2fv1_2fprefix_2eproto_getter, &descriptor_table_proto_2fgateway_2fv1_2fprefix_2eproto_once,
      file_level_metadata_proto_2fgateway_2fv1_2fprefix_2eproto[1]);
}
// ===================================================================

class Prefix::_Internal {
 public:
};

Prefix::Prefix(::google::protobuf::Arena* arena)
    : ::google::protobuf::Message(arena) {
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:proto.gateway.v1.Prefix)
}
inline PROTOBUF_NDEBUG_INLINE Prefix::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility, ::google::protobuf::Arena* arena,
    const Impl_& from)
      : prefix_(arena, from.prefix_),
        _cached_size_{0} {}

Prefix::Prefix(
    ::google::protobuf::Arena* arena,
    const Prefix& from)
    : ::google::protobuf::Message(arena) {
  Prefix* const _this = this;
  (void)_this;
  _internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(
      from._internal_metadata_);
  new (&_impl_) Impl_(internal_visibility(), arena, from._impl_);
  _impl_.mask_ = from._impl_.mask_;

  // @@protoc_insertion_point(copy_constructor:proto.gateway.v1.Prefix)
}
inline PROTOBUF_NDEBUG_INLINE Prefix::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility,
    ::google::protobuf::Arena* arena)
      : prefix_(arena),
        _cached_size_{0} {}

inline void Prefix::SharedCtor(::_pb::Arena* arena) {
  new (&_impl_) Impl_(internal_visibility(), arena);
  _impl_.mask_ = {};
}
Prefix::~Prefix() {
  // @@protoc_insertion_point(destructor:proto.gateway.v1.Prefix)
  _internal_metadata_.Delete<::google::protobuf::UnknownFieldSet>();
  SharedDtor();
}
inline void Prefix::SharedDtor() {
  ABSL_DCHECK(GetArena() == nullptr);
  _impl_.prefix_.Destroy();
  _impl_.~Impl_();
}

PROTOBUF_NOINLINE void Prefix::Clear() {
// @@protoc_insertion_point(message_clear_start:proto.gateway.v1.Prefix)
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.prefix_.ClearToEmpty();
  _impl_.mask_ = 0u;
  _internal_metadata_.Clear<::google::protobuf::UnknownFieldSet>();
}

const char* Prefix::_InternalParse(
    const char* ptr, ::_pbi::ParseContext* ctx) {
  ptr = ::_pbi::TcParser::ParseLoop(this, ptr, ctx, &_table_.header);
  return ptr;
}


PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::_pbi::TcParseTable<1, 2, 0, 0, 2> Prefix::_table_ = {
  {
    0,  // no _has_bits_
    0, // no _extensions_
    2, 8,  // max_field_number, fast_idx_mask
    offsetof(decltype(_table_), field_lookup_table),
    4294967292,  // skipmap
    offsetof(decltype(_table_), field_entries),
    2,  // num_field_entries
    0,  // num_aux_entries
    offsetof(decltype(_table_), field_names),  // no aux_entries
    &_Prefix_default_instance_._instance,
    ::_pbi::TcParser::GenericFallback,  // fallback
  }, {{
    // uint32 mask = 2;
    {::_pbi::TcParser::SingularVarintNoZag1<::uint32_t, offsetof(Prefix, _impl_.mask_), 63>(),
     {16, 63, 0, PROTOBUF_FIELD_OFFSET(Prefix, _impl_.mask_)}},
    // bytes prefix = 1;
    {::_pbi::TcParser::FastBS1,
     {10, 63, 0, PROTOBUF_FIELD_OFFSET(Prefix, _impl_.prefix_)}},
  }}, {{
    65535, 65535
  }}, {{
    // bytes prefix = 1;
    {PROTOBUF_FIELD_OFFSET(Prefix, _impl_.prefix_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kBytes | ::_fl::kRepAString)},
    // uint32 mask = 2;
    {PROTOBUF_FIELD_OFFSET(Prefix, _impl_.mask_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kUInt32)},
  }},
  // no aux_entries
  {{
  }},
};

::uint8_t* Prefix::_InternalSerialize(
    ::uint8_t* target,
    ::google::protobuf::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:proto.gateway.v1.Prefix)
  ::uint32_t cached_has_bits = 0;
  (void)cached_has_bits;

  // bytes prefix = 1;
  if (!this->_internal_prefix().empty()) {
    const std::string& _s = this->_internal_prefix();
    target = stream->WriteBytesMaybeAliased(1, _s, target);
  }

  // uint32 mask = 2;
  if (this->_internal_mask() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteUInt32ToArray(
        2, this->_internal_mask(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target =
        ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
            _internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:proto.gateway.v1.Prefix)
  return target;
}

::size_t Prefix::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:proto.gateway.v1.Prefix)
  ::size_t total_size = 0;

  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // bytes prefix = 1;
  if (!this->_internal_prefix().empty()) {
    total_size += 1 + ::google::protobuf::internal::WireFormatLite::BytesSize(
                                    this->_internal_prefix());
  }

  // uint32 mask = 2;
  if (this->_internal_mask() != 0) {
    total_size += ::_pbi::WireFormatLite::UInt32SizePlusOne(
        this->_internal_mask());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::google::protobuf::Message::ClassData Prefix::_class_data_ = {
    Prefix::MergeImpl,
    nullptr,  // OnDemandRegisterArenaDtor
};
const ::google::protobuf::Message::ClassData* Prefix::GetClassData() const {
  return &_class_data_;
}

void Prefix::MergeImpl(::google::protobuf::Message& to_msg, const ::google::protobuf::Message& from_msg) {
  auto* const _this = static_cast<Prefix*>(&to_msg);
  auto& from = static_cast<const Prefix&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:proto.gateway.v1.Prefix)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_prefix().empty()) {
    _this->_internal_set_prefix(from._internal_prefix());
  }
  if (from._internal_mask() != 0) {
    _this->_internal_set_mask(from._internal_mask());
  }
  _this->_internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(from._internal_metadata_);
}

void Prefix::CopyFrom(const Prefix& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:proto.gateway.v1.Prefix)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

PROTOBUF_NOINLINE bool Prefix::IsInitialized() const {
  return true;
}

::_pbi::CachedSize* Prefix::AccessCachedSize() const {
  return &_impl_._cached_size_;
}
void Prefix::InternalSwap(Prefix* PROTOBUF_RESTRICT other) {
  using std::swap;
  auto* arena = GetArena();
  ABSL_DCHECK_EQ(arena, other->GetArena());
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.prefix_, &other->_impl_.prefix_, arena);
        swap(_impl_.mask_, other->_impl_.mask_);
}

::google::protobuf::Metadata Prefix::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_proto_2fgateway_2fv1_2fprefix_2eproto_getter, &descriptor_table_proto_2fgateway_2fv1_2fprefix_2eproto_once,
      file_level_metadata_proto_2fgateway_2fv1_2fprefix_2eproto[2]);
}
// @@protoc_insertion_point(namespace_scope)
}  // namespace v1
}  // namespace gateway
}  // namespace proto
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google
// @@protoc_insertion_point(global_scope)
#include "google/protobuf/port_undef.inc"
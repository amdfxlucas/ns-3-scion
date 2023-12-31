// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: proto/gateway/v1/prefix.proto
// Original file comments:
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
//
#ifndef GRPC_proto_2fgateway_2fv1_2fprefix_2eproto__INCLUDED
#define GRPC_proto_2fgateway_2fv1_2fprefix_2eproto__INCLUDED

#include "proto/gateway/v1/prefix.pb.h"

#include <functional>
#include <grpcpp/generic/async_generic_service.h>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/client_context.h>
#include <grpcpp/completion_queue.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/proto_utils.h>
#include <grpcpp/impl/rpc_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/status.h>
#include <grpcpp/support/stub_options.h>
#include <grpcpp/support/sync_stream.h>

namespace proto {
namespace gateway {
namespace v1 {

class IPPrefixesService final {
 public:
  static constexpr char const* service_full_name() {
    return "proto.gateway.v1.IPPrefixesService";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    // Prefixes requests the IP prefixes that can be reachable via the remote.
    virtual ::grpc::Status Prefixes(::grpc::ClientContext* context, const ::proto::gateway::v1::PrefixesRequest& request, ::proto::gateway::v1::PrefixesResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::proto::gateway::v1::PrefixesResponse>> AsyncPrefixes(::grpc::ClientContext* context, const ::proto::gateway::v1::PrefixesRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::proto::gateway::v1::PrefixesResponse>>(AsyncPrefixesRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::proto::gateway::v1::PrefixesResponse>> PrepareAsyncPrefixes(::grpc::ClientContext* context, const ::proto::gateway::v1::PrefixesRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::proto::gateway::v1::PrefixesResponse>>(PrepareAsyncPrefixesRaw(context, request, cq));
    }
    class async_interface {
     public:
      virtual ~async_interface() {}
      // Prefixes requests the IP prefixes that can be reachable via the remote.
      virtual void Prefixes(::grpc::ClientContext* context, const ::proto::gateway::v1::PrefixesRequest* request, ::proto::gateway::v1::PrefixesResponse* response, std::function<void(::grpc::Status)>) = 0;
      virtual void Prefixes(::grpc::ClientContext* context, const ::proto::gateway::v1::PrefixesRequest* request, ::proto::gateway::v1::PrefixesResponse* response, ::grpc::ClientUnaryReactor* reactor) = 0;
    };
    typedef class async_interface experimental_async_interface;
    virtual class async_interface* async() { return nullptr; }
    class async_interface* experimental_async() { return async(); }
   private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::proto::gateway::v1::PrefixesResponse>* AsyncPrefixesRaw(::grpc::ClientContext* context, const ::proto::gateway::v1::PrefixesRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::proto::gateway::v1::PrefixesResponse>* PrepareAsyncPrefixesRaw(::grpc::ClientContext* context, const ::proto::gateway::v1::PrefixesRequest& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());
    ::grpc::Status Prefixes(::grpc::ClientContext* context, const ::proto::gateway::v1::PrefixesRequest& request, ::proto::gateway::v1::PrefixesResponse* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::proto::gateway::v1::PrefixesResponse>> AsyncPrefixes(::grpc::ClientContext* context, const ::proto::gateway::v1::PrefixesRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::proto::gateway::v1::PrefixesResponse>>(AsyncPrefixesRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::proto::gateway::v1::PrefixesResponse>> PrepareAsyncPrefixes(::grpc::ClientContext* context, const ::proto::gateway::v1::PrefixesRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::proto::gateway::v1::PrefixesResponse>>(PrepareAsyncPrefixesRaw(context, request, cq));
    }
    class async final :
      public StubInterface::async_interface {
     public:
      void Prefixes(::grpc::ClientContext* context, const ::proto::gateway::v1::PrefixesRequest* request, ::proto::gateway::v1::PrefixesResponse* response, std::function<void(::grpc::Status)>) override;
      void Prefixes(::grpc::ClientContext* context, const ::proto::gateway::v1::PrefixesRequest* request, ::proto::gateway::v1::PrefixesResponse* response, ::grpc::ClientUnaryReactor* reactor) override;
     private:
      friend class Stub;
      explicit async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class async* async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class async async_stub_{this};
    ::grpc::ClientAsyncResponseReader< ::proto::gateway::v1::PrefixesResponse>* AsyncPrefixesRaw(::grpc::ClientContext* context, const ::proto::gateway::v1::PrefixesRequest& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::proto::gateway::v1::PrefixesResponse>* PrepareAsyncPrefixesRaw(::grpc::ClientContext* context, const ::proto::gateway::v1::PrefixesRequest& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_Prefixes_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    // Prefixes requests the IP prefixes that can be reachable via the remote.
    virtual ::grpc::Status Prefixes(::grpc::ServerContext* context, const ::proto::gateway::v1::PrefixesRequest* request, ::proto::gateway::v1::PrefixesResponse* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_Prefixes : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_Prefixes() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_Prefixes() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Prefixes(::grpc::ServerContext* /*context*/, const ::proto::gateway::v1::PrefixesRequest* /*request*/, ::proto::gateway::v1::PrefixesResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestPrefixes(::grpc::ServerContext* context, ::proto::gateway::v1::PrefixesRequest* request, ::grpc::ServerAsyncResponseWriter< ::proto::gateway::v1::PrefixesResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_Prefixes<Service > AsyncService;
  template <class BaseClass>
  class WithCallbackMethod_Prefixes : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_Prefixes() {
      ::grpc::Service::MarkMethodCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::proto::gateway::v1::PrefixesRequest, ::proto::gateway::v1::PrefixesResponse>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::proto::gateway::v1::PrefixesRequest* request, ::proto::gateway::v1::PrefixesResponse* response) { return this->Prefixes(context, request, response); }));}
    void SetMessageAllocatorFor_Prefixes(
        ::grpc::MessageAllocator< ::proto::gateway::v1::PrefixesRequest, ::proto::gateway::v1::PrefixesResponse>* allocator) {
      ::grpc::internal::MethodHandler* const handler = ::grpc::Service::GetHandler(0);
      static_cast<::grpc::internal::CallbackUnaryHandler< ::proto::gateway::v1::PrefixesRequest, ::proto::gateway::v1::PrefixesResponse>*>(handler)
              ->SetMessageAllocator(allocator);
    }
    ~WithCallbackMethod_Prefixes() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Prefixes(::grpc::ServerContext* /*context*/, const ::proto::gateway::v1::PrefixesRequest* /*request*/, ::proto::gateway::v1::PrefixesResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* Prefixes(
      ::grpc::CallbackServerContext* /*context*/, const ::proto::gateway::v1::PrefixesRequest* /*request*/, ::proto::gateway::v1::PrefixesResponse* /*response*/)  { return nullptr; }
  };
  typedef WithCallbackMethod_Prefixes<Service > CallbackService;
  typedef CallbackService ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_Prefixes : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_Prefixes() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_Prefixes() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Prefixes(::grpc::ServerContext* /*context*/, const ::proto::gateway::v1::PrefixesRequest* /*request*/, ::proto::gateway::v1::PrefixesResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_Prefixes : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_Prefixes() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_Prefixes() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Prefixes(::grpc::ServerContext* /*context*/, const ::proto::gateway::v1::PrefixesRequest* /*request*/, ::proto::gateway::v1::PrefixesResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestPrefixes(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_Prefixes : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_Prefixes() {
      ::grpc::Service::MarkMethodRawCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::grpc::ByteBuffer* request, ::grpc::ByteBuffer* response) { return this->Prefixes(context, request, response); }));
    }
    ~WithRawCallbackMethod_Prefixes() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Prefixes(::grpc::ServerContext* /*context*/, const ::proto::gateway::v1::PrefixesRequest* /*request*/, ::proto::gateway::v1::PrefixesResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* Prefixes(
      ::grpc::CallbackServerContext* /*context*/, const ::grpc::ByteBuffer* /*request*/, ::grpc::ByteBuffer* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_Prefixes : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithStreamedUnaryMethod_Prefixes() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler<
          ::proto::gateway::v1::PrefixesRequest, ::proto::gateway::v1::PrefixesResponse>(
            [this](::grpc::ServerContext* context,
                   ::grpc::ServerUnaryStreamer<
                     ::proto::gateway::v1::PrefixesRequest, ::proto::gateway::v1::PrefixesResponse>* streamer) {
                       return this->StreamedPrefixes(context,
                         streamer);
                  }));
    }
    ~WithStreamedUnaryMethod_Prefixes() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status Prefixes(::grpc::ServerContext* /*context*/, const ::proto::gateway::v1::PrefixesRequest* /*request*/, ::proto::gateway::v1::PrefixesResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedPrefixes(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::proto::gateway::v1::PrefixesRequest,::proto::gateway::v1::PrefixesResponse>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_Prefixes<Service > StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef WithStreamedUnaryMethod_Prefixes<Service > StreamedService;
};

}  // namespace v1
}  // namespace gateway
}  // namespace proto


#endif  // GRPC_proto_2fgateway_2fv1_2fprefix_2eproto__INCLUDED

// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: proto/discovery/v1/discovery.proto

#include "proto/discovery/v1/discovery.pb.h"
#include "proto/discovery/v1/discovery.grpc.pb.h"

#include <functional>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/impl/client_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/rpc_service_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/sync_stream.h>
namespace proto {
namespace discovery {
namespace v1 {

static const char* DiscoveryService_method_names[] = {
  "/proto.discovery.v1.DiscoveryService/Gateways",
  "/proto.discovery.v1.DiscoveryService/HiddenSegmentServices",
};

std::unique_ptr< DiscoveryService::Stub> DiscoveryService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< DiscoveryService::Stub> stub(new DiscoveryService::Stub(channel, options));
  return stub;
}

DiscoveryService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_Gateways_(DiscoveryService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_HiddenSegmentServices_(DiscoveryService_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status DiscoveryService::Stub::Gateways(::grpc::ClientContext* context, const ::proto::discovery::v1::GatewaysRequest& request, ::proto::discovery::v1::GatewaysResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::proto::discovery::v1::GatewaysRequest, ::proto::discovery::v1::GatewaysResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Gateways_, context, request, response);
}

void DiscoveryService::Stub::async::Gateways(::grpc::ClientContext* context, const ::proto::discovery::v1::GatewaysRequest* request, ::proto::discovery::v1::GatewaysResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::proto::discovery::v1::GatewaysRequest, ::proto::discovery::v1::GatewaysResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Gateways_, context, request, response, std::move(f));
}

void DiscoveryService::Stub::async::Gateways(::grpc::ClientContext* context, const ::proto::discovery::v1::GatewaysRequest* request, ::proto::discovery::v1::GatewaysResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Gateways_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::proto::discovery::v1::GatewaysResponse>* DiscoveryService::Stub::PrepareAsyncGatewaysRaw(::grpc::ClientContext* context, const ::proto::discovery::v1::GatewaysRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::proto::discovery::v1::GatewaysResponse, ::proto::discovery::v1::GatewaysRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Gateways_, context, request);
}

::grpc::ClientAsyncResponseReader< ::proto::discovery::v1::GatewaysResponse>* DiscoveryService::Stub::AsyncGatewaysRaw(::grpc::ClientContext* context, const ::proto::discovery::v1::GatewaysRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGatewaysRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status DiscoveryService::Stub::HiddenSegmentServices(::grpc::ClientContext* context, const ::proto::discovery::v1::HiddenSegmentServicesRequest& request, ::proto::discovery::v1::HiddenSegmentServicesResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::proto::discovery::v1::HiddenSegmentServicesRequest, ::proto::discovery::v1::HiddenSegmentServicesResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_HiddenSegmentServices_, context, request, response);
}

void DiscoveryService::Stub::async::HiddenSegmentServices(::grpc::ClientContext* context, const ::proto::discovery::v1::HiddenSegmentServicesRequest* request, ::proto::discovery::v1::HiddenSegmentServicesResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::proto::discovery::v1::HiddenSegmentServicesRequest, ::proto::discovery::v1::HiddenSegmentServicesResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_HiddenSegmentServices_, context, request, response, std::move(f));
}

void DiscoveryService::Stub::async::HiddenSegmentServices(::grpc::ClientContext* context, const ::proto::discovery::v1::HiddenSegmentServicesRequest* request, ::proto::discovery::v1::HiddenSegmentServicesResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_HiddenSegmentServices_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::proto::discovery::v1::HiddenSegmentServicesResponse>* DiscoveryService::Stub::PrepareAsyncHiddenSegmentServicesRaw(::grpc::ClientContext* context, const ::proto::discovery::v1::HiddenSegmentServicesRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::proto::discovery::v1::HiddenSegmentServicesResponse, ::proto::discovery::v1::HiddenSegmentServicesRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_HiddenSegmentServices_, context, request);
}

::grpc::ClientAsyncResponseReader< ::proto::discovery::v1::HiddenSegmentServicesResponse>* DiscoveryService::Stub::AsyncHiddenSegmentServicesRaw(::grpc::ClientContext* context, const ::proto::discovery::v1::HiddenSegmentServicesRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncHiddenSegmentServicesRaw(context, request, cq);
  result->StartCall();
  return result;
}

DiscoveryService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      DiscoveryService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< DiscoveryService::Service, ::proto::discovery::v1::GatewaysRequest, ::proto::discovery::v1::GatewaysResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](DiscoveryService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::proto::discovery::v1::GatewaysRequest* req,
             ::proto::discovery::v1::GatewaysResponse* resp) {
               return service->Gateways(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      DiscoveryService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< DiscoveryService::Service, ::proto::discovery::v1::HiddenSegmentServicesRequest, ::proto::discovery::v1::HiddenSegmentServicesResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](DiscoveryService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::proto::discovery::v1::HiddenSegmentServicesRequest* req,
             ::proto::discovery::v1::HiddenSegmentServicesResponse* resp) {
               return service->HiddenSegmentServices(ctx, req, resp);
             }, this)));
}

DiscoveryService::Service::~Service() {
}

::grpc::Status DiscoveryService::Service::Gateways(::grpc::ServerContext* context, const ::proto::discovery::v1::GatewaysRequest* request, ::proto::discovery::v1::GatewaysResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status DiscoveryService::Service::HiddenSegmentServices(::grpc::ServerContext* context, const ::proto::discovery::v1::HiddenSegmentServicesRequest* request, ::proto::discovery::v1::HiddenSegmentServicesResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace proto
}  // namespace discovery
}  // namespace v1


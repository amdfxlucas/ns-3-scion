find_external_library(DEPENDENCY_NAME yaml-cpp
                      HEADER_NAME yaml.h
                      LIBRARY_NAME yaml-cpp
                      SEARCH_PATHS /usr/local )
if(${yaml-cpp_FOUND})
    include_directories(${yaml-cpp_INCLUDE_DIRS})
    link_libraries(${yaml-cpp_LIBRARIES})
endif()

include_directories(   /usr/local/include/neo/buffer
                       /usr/local/include/neo/fun
                      /usr/local/include/neo 
                      /usr/local/include 
                      /usr/local/include/neo/fun/neo 
                      /usr/local/include/any_iterator
                     # /usr/include/c++/13
                      )


list(APPEND CMAKE_PREFIX_PATH "/usr/local/lib/cmake/neo" "/usr/local/lib/cmake/any_iterator")
find_package(neo-buffer CONFIG REQUIRED )
find_package(fun  CONFIG REQUIRED )
find_package(any_iterator CONFIG REQUIRED )

#find_external_library(DEPENDENCY_NAME protobuf
#                     # HEADER_NAME yaml.h
#                      LIBRARY_NAME protobuf
#                      SEARCH_PATHS /usr/local )
#if(${protobuf_FOUND})
#    include_directories(${protobuf_INCLUDE_DIRS})
#    link_libraries(${protobuf_LIBRARIES})
#endif()


  option(protobuf_MODULE_COMPATIBLE TRUE)
  find_package(Protobuf CONFIG REQUIRED)
  message(STATUS "Using protobuf ${Protobuf_VERSION}")
  #message(STATUS "PROTOBUF_LIBRARIES: ${protobuf::libprotobuf}")

  set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
#  set(_REFLECTION gRPC::grpc++_reflection)
#  if(CMAKE_CROSSCOMPILING)
#    find_program(_PROTOBUF_PROTOC protoc)
#  else()
#    set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
#  endif()

#include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/proto/control_plane/v1" )

build_lib(
  LIBNAME SCION
  SOURCE_FILES
    model/beaconing/baseline.cc
    model/beaconing/beacon.cc
    model/beaconing/beacon-server.cc
    model/beaconing/diversity-age-based.cc
    model/beaconing/latency-optimized-beaconing.cc
    model/beaconing/green-beaconing.cc
    model/beaconing/on-demand-optimization.cc
    model/post-simulation-evaluations.cc    
    model/user-defined-events.cc
    model/border-router-application.cc
    #model/packet-processor.cc
    model/pre-simulation-setup.cc
    model/path-server.cc
    model/path-segment.cc
    model/scion-packet.cc
    model/scion-host.cc
    model/scion-as.cc
    model/scion-core-as.cc
    model/utils.cc
    model/scion-capable-node.cc
    model/border-router.cc
    model/beaconing/scionlab-algo.cc
    model/time-server.cc
    model/scion-global-scheduling.cc

    model/ns-3-style/base-field.cc    
    model/ns-3-style/decoded-path.cc
    model/ns-3-style/hop-field.cc 
    # model/ns-3-style/one-hop-path.cc
    model/ns-3-style/raw-path.cc
    model/ns-3-style/info-field.cc
    model/ns-3-style/gopacket++/udp-layer.cc
    model/ns-3-style/scion-header.cc
    model/ns-3-style/scion-address.cc
    model/ns-3-style/gopacket++/gopacket++.cc
    model/ns-3-style/go-errors/basic-error.cc
    model/ns-3-style/socket/scion-udp-socket-impl.cc
    model/ns-3-style/socket/scion-udp-socket-impl-unchanged.cc
    model/ns-3-style/scion-snet-path-meta-data.cc
    model/ns-3-style/scion-default-path-selector.cc
    model/ns-3-style/scion-default-reply-selector.cc

    model/scion-simulation-context.cc
    model/util/sha256.cc
    model/util/string-utils.cc

    model/ns-3-style/scmp/scmp-packet-too-big.cc
    model/ns-3-style/scmp/scmp-echo.cc
    model/ns-3-style/scmp/scmp-destination-unreachable.cc
    model/ns-3-style/scmp/scmp-internal-connectivity-down.cc
    model/ns-3-style/scmp/scmp-parameter-problem.cc
    model/ns-3-style/scmp/scmp-traceroute.cc
    model/ns-3-style/scmp/scmp-external-interface-down.cc
    model/ns-3-style/scmp/scmp-header.cc
    model/ns-3-style/extn/tlv_option.cc
    model/ns-3-style/extn/extn_base.cc
    model/ns-3-style/extn/hop-by-hop-extn.cc
    model/ns-3-style/extn/end-to-end-extn.cc




  HEADER_FILES
  
  model/util/scion-utils.h
  model/util/string-utils.h
  model/util/sha256.h
  model/util/range_ref.h
  model/util/function_ref.h
  model/util/iterator-util.h
  model/util/endian-utils.h
  model/util/placeholder.h
  
  model/ns-3-style/l4-address.h

  model/ns-3-style/scion-reply-selector.h
  model/ns-3-style/scion-stack/scion-stack-helper.h
  model/ns-3-style/scion-stack/scion-stack.h
  model/ns-3-style/scion-path-policy.h
  
  model/ns-3-style/scion-path-selector.h
  model/ns-3-style/scion-default-path-selector.h
  model/ns-3-style/scion-snet-path-meta-data.h
  model/ns-3-style/scion-snet-path.h

  model/ns-3-style/extn/tlv_option.h
  model/ns-3-style/extn/hop-by-hop-extn.h
  model/ns-3-style/extn/end-to-end-extn.h
  model/ns-3-style/extn/extn_base.h
  model/ns-3-style/scmp/scmp-packet-too-big.h
  model/ns-3-style/scmp/scmp-echo.h
  model/ns-3-style/scmp/scmp-destination-unreachable.h
  model/ns-3-style/scmp/scmp-traceroute.h
  model/ns-3-style/scmp/scmp-internal-connectivity-down.h
  model/ns-3-style/scmp/scmp-parameter-problem.h
  model/ns-3-style/scmp/scmp-external-interface-down.h
  model/ns-3-style/scmp/scmp-header.h
  model/ns-3-style/socket/scion-udp-socket-impl.h  
  model/ns-3-style/scion-types.h
  model/ns-3-style/info-field.h
  model/ns-3-style/raw-path.h
  model/ns-3-style/isd-as.h
  model/ns-3-style/scion-address.h 
  model/ns-3-style/hop-field.h
  model/ns-3-style/decoded-path.h
  model/ns-3-style/fields-forward.h
  model/ns-3-style/scion-header.h
  model/ns-3-style/gopacket++/udp-layer.h
  model/ns-3-style/scion-packet.h
  model/ns-3-style/scmp/scmp-typecodes.h
  model/ns-3-style/scion-path.h
  # model/ns-3-style/one-hop-path.h
  model/ns-3-style/base-field.h
  model/ns-3-style/gopacket++/gopacket++.h
  model/ns-3-style/go-errors/go-errors.h
  model/ns-3-style/go-errors/basic-error.h
  model/ns-3-style/go-errors/scmp-error.h
  
    model/beaconing/baseline.h
    model/beaconing/beacon.h
    model/beaconing/beacon-server.h
    model/beaconing/diversity-age-based.h
    model/beaconing/latency-optimized-beaconing.h
    model/beaconing/green-beaconing.h
    model/beaconing/on-demand-optimization.h
    model/post-simulation-evaluations.h
    model/border-router-application.h
    #model/packet-processor.h
    model/run-parallel-events.h
    model/user-defined-events.h
    model/pre-simulation-setup.h
    model/path-server.h
    model/path-segment.h
    model/scion-packet.h
    model/scion-host.h
    model/scion-as.h
    model/scion-core-as.h
    model/scion-capable-node.h
    model/border-router.h
    model/utils.h
    model/beaconing/scionlab-algo.h
    model/time-server.h
    model/scion-simulation-context.h

    model/json.hpp


  LIBRARIES_TO_LINK 
  ${libnetwork}
   ${libcore} 
   ${libinternet}
     ${libtopology-read}
     neo::neo-buffer
     neo::fun
   #  protobuf::libprotobuf 
   
  TEST_SOURCES 
  model/ns-3-style/test/scion-test-data.h
  model/ns-3-style/test/scion-address-test.cc
  model/ns-3-style/test/scion-header-test.cc
  model/ns-3-style/test/scion-path-test.cc
  model/ns-3-style/test/extn-test.cc
  model/ns-3-style/test/decoded-test.cc
  model/ns-3-style/test/raw-path-test.cc
  model/ns-3-style/test/go-errors-test.cc
   
)

#target_include_directories(  libSCION-obj PUBLIC /usr/local/include/neo/buffer /usr/local/include/neo/fun /usr/local/include/neo /usr/local/include /usr/local/include/neo/fun/neo )



set(scion_protobuf_header     # -------------- PROTOBUF -------------------------------------
proto/discovery/v1/discovery.pb.h
#proto/discovery/v1/discovery.grpc.pb.h
#proto/drkey/v1/drkey.grpc.pb.h
proto/drkey/v1/drkey.pb.h
proto/crypto/v1/signed.pb.h
#proto/crypto/v1/signed.grpc.pb.h
proto/daemon/v1/daemon.pb.h
#proto/daemon/v1/daemon.grpc.pb.h
proto/hidden_segment/v1/hidden_segment.pb.h
#proto/hidden_segment/v1/hidden_segment.grpc.pb.h
proto/control_plane/experimental/v1/seg_detached_extensions.pb.h
#proto/control_plane/experimental/v1/seg_detached_extensions.grpc.pb.h
#proto/control_plane/v1/seg_extensions.grpc.pb.h
proto/control_plane/v1/seg.pb.h
proto/control_plane/v1/svc_resolution.pb.h
#proto/control_plane/v1/seg.grpc.pb.h
#proto/control_plane/v1/cppki.grpc.pb.h
proto/control_plane/v1/seg_extensions.pb.h
proto/control_plane/v1/renewal.pb.h
# proto/control_plane/v1/renewal.grpc.pb.h
# proto/control_plane/v1/drkey.grpc.pb.h
proto/control_plane/v1/drkey.pb.h
# proto/control_plane/v1/svc_resolution.grpc.pb.h
proto/control_plane/v1/cppki.pb.h
#proto/gateway/v1/prefix.grpc.pb.h
proto/gateway/v1/control.pb.h
#proto/gateway/v1/control.grpc.pb.h
proto/gateway/v1/prefix.pb.h
)

set(scion_protobuf_sources    # --- PROTOBUF --------------------

#proto/discovery/v1/discovery.grpc.pb.cc
proto/discovery/v1/discovery.pb.cc
proto/drkey/v1/drkey.pb.cc
#proto/drkey/v1/drkey.grpc.pb.cc
proto/crypto/v1/signed.pb.cc
#proto/crypto/v1/signed.grpc.pb.cc
#proto/daemon/v1/daemon.grpc.pb.cc
proto/daemon/v1/daemon.pb.cc
proto/hidden_segment/v1/hidden_segment.pb.cc
#proto/hidden_segment/v1/hidden_segment.grpc.pb.cc
proto/control_plane/experimental/v1/seg_detached_extensions.pb.cc
#proto/control_plane/experimental/v1/seg_detached_extensions.grpc.pb.cc
#proto/control_plane/v1/cppki.grpc.pb.cc
proto/control_plane/v1/renewal.pb.cc
proto/control_plane/v1/svc_resolution.pb.cc
#proto/control_plane/v1/seg_extensions.grpc.pb.cc
#proto/control_plane/v1/seg.grpc.pb.cc
proto/control_plane/v1/seg.pb.cc
proto/control_plane/v1/seg_extensions.pb.cc
#proto/control_plane/v1/renewal.grpc.pb.cc
proto/control_plane/v1/drkey.pb.cc
#proto/control_plane/v1/svc_resolution.grpc.pb.cc
#proto/control_plane/v1/drkey.grpc.pb.cc
proto/control_plane/v1/cppki.pb.cc
proto/gateway/v1/prefix.pb.cc
#proto/gateway/v1/prefix.grpc.pb.cc
#proto/gateway/v1/control.grpc.pb.cc
proto/gateway/v1/control.pb.cc
)

#add_subdirectory(abseil-cpp)
#target_link_libraries(libSCION-obj PUBLIC absl::base absl::synchronization absl::strings absl::log)
#absl::algorithm
#absl::base
#absl::debugging
#absl::flat_hash_map
#absl::flags
#absl::memory
#absl::meta
#absl::numeric
#absl::random_random
#absl::strings
#absl::synchronization
#absl::time
#absl::utility



#build_lib(
#  LIBNAME SCIONPROTO
#  SOURCE_FILES
#   ${scion_protobuf_sources}
#  HEADER_FILES
#   ${scion_protobuf_headers}
#   LIBRARIES_TO_LINK
#  TEST_SOURCES
#)

#add_library( libscion_protobuf ${scion_protobuf_sources} ${scion_protobuf_header} )  # previous version

add_library(libscion_protobuf)
#target_sources( neo-fun PRIVATE ${SOURCES} )
target_sources( libscion_protobuf PRIVATE ${scion_protobuf_sources} PUBLIC FILE_SET HEADERS 
BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}/proto
FILES ${scion_protobuf_header}
)

#target_include_directories(fun PUBLIC ${PROJECT_SOURCE_DIR}/src )
target_include_directories( libscion_protobuf PUBLIC  
$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/proto>  
#$<INSTALL_INTERFACE:/usr/local/include/neo/fun>  # <prefix>/include/mylib) 
)

# Your-external "mylib", add GLOBAL if the imported library is located in directories above the current.
#add_library( protobuff SHARED IMPORTED GLOBAL)
#set_target_properties( protobuff PROPERTIES IMPORTED_LOCATION /usr/local/lib/libprotobuf.so )
#target_link_libraries( libscion_protobuf protobuff )
#target_include_directories( libscion_protobuf PUBLIC "/usr/local/include/google")
target_link_libraries( libscion_protobuf protobuf::libprotobuf )
#set(ns3-contrib-libs "scion_protobuf;${ns3-contrib-libs}" CACHE INTERNAL "list of processed contrib modules")

set(ns3-libs "libscion_protobuf;${ns3-libs}" CACHE INTERNAL "list of processed upstream modules")

#link_libraries(  protobuf::libprotobuf  )# links to static .a for some reason
target_link_libraries( libSCION-obj   protobuf::libprotobuf libscion_protobuf )

target_include_directories( libscion_protobuf PRIVATE /home/lucas/repos/ns-3-scion/src/SCION )
# "${CMAKE_CURRENT_SOURCE_DIR}/proto/control_plane/v1" ) # lib-ns3
#target_include_directories( libSCION-obj PUBLIC /usr/include/c++/13 )

target_compile_definitions( libSCION-obj PRIVATE ERRORS_WITH_CONTEXT=1)
message(status ${test${BLIB_LIBNAME}} )
if(${ENABLE_TESTS} )
target_compile_definitions( libSCION-test PRIVATE ERRORS_WITH_CONTEXT=1 )
endif()

#target_compile_definitions(libSCION-obj PRIVATE -fconcepts-diagnostics-depth=10 )

#build_example(
#  NAME scion-example
#  SOURCE_FILES model/ns-3-style/examples/socket-example.cc
#  LIBRARIES_TO_LINK
#    ${libSCION}    
#    ${libpoint-to-point}
#)

build_exec(
STANDALONE
  EXECNAME scion-sim  
  SOURCE_FILES scion.cc  
  LIBRARIES_TO_LINK ${libSCION}
                    ${libtopology-read}
                    ${libpoint-to-point}
                  #  protobuf::libprotobuf 
                  libscion_protobuf
                 # protobuff
                 protobuf::libprotobuf
                # absl::log
  EXECUTABLE_DIRECTORY_PATH ${CMAKE_OUTPUT_DIRECTORY}/scion/bin
)


#target_link_libraries( scion-sim   protobuf::libprotobuf )
#target_link_libraries(scion-sim protobuff)
#target_include_directories(scion-sim PUBLIC "/usr/local/include/google/protobuf")
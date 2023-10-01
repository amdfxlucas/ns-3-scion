find_external_library(DEPENDENCY_NAME yaml-cpp
                      HEADER_NAME yaml.h
                      LIBRARY_NAME yaml-cpp
                      SEARCH_PATHS /usr/local )
if(${yaml-cpp_FOUND})
    include_directories(${yaml-cpp_INCLUDE_DIRS})
    link_libraries(${yaml-cpp_LIBRARIES})
endif()

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
    model/schedule-periodic-events.cc
    model/user-defined-events.cc
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
    model/externs.cc
  HEADER_FILES
    model/beaconing/baseline.h
    model/beaconing/beacon.h
    model/beaconing/beacon-server.h
    model/beaconing/diversity-age-based.h
    model/beaconing/latency-optimized-beaconing.h
    model/beaconing/green-beaconing.h
    model/beaconing/on-demand-optimization.h
    model/post-simulation-evaluations.h
    model/schedule-periodic-events.h
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
    model/externs.h
    model/json.hpp
  LIBRARIES_TO_LINK ${libnetwork} ${libcore}
 # TEST_SOURCES # TODO
 #   test/some_scion_tests.cc
   
)

#build_lib_example(
#  NAME scion
#  SOURCE_FILES scion.cc
#  LIBRARIES_TO_LINK
#    ${libSCION}    
#    ${libpoint-to-point}
#)

build_exec(
STANDALONE
  EXECNAME scion-sim  
  SOURCE_FILES scion.cc  
  LIBRARIES_TO_LINK ${libSCION}
                    ${libpoint-to-point}
  EXECUTABLE_DIRECTORY_PATH ${CMAKE_OUTPUT_DIRECTORY}/scion/bin
)
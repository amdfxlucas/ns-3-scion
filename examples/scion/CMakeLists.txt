build_example(
  NAME scion-example
  SOURCE_FILES socket-example.cc
  LIBRARIES_TO_LINK
    ${libSCION}    
    ${libpoint-to-point}
)
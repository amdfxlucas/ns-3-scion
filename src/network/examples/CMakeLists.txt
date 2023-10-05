set(base_examples
    bit-serializer
    main-packet-header
    main-packet-tag
    packet-socket-apps
    lollipop-comparisons
    #src/network/examples/
    packet-socket-experiment
)

foreach(
  example
  ${base_examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES ${example}.cc
    LIBRARIES_TO_LINK ${libnetwork}
                      ${libcore}
                      ${libpoint-to-point}
  )
endforeach()

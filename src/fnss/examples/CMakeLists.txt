build_lib_example(
  NAME fnss-example
  SOURCE_FILES fnss-example.cc
  LIBRARIES_TO_LINK
    ${libfnss}
    # transitive ?! ${libnetwork} ${libcore} ${libinternet} ${libpoint-to-point} ${libapplications}
)

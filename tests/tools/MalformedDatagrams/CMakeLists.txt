add_library(rtype_tools_malformed_datagrams_constants INTERFACE)

target_include_directories(rtype_tools_malformed_datagrams_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_tools_malformed_datagrams_constants INTERFACE rtype_network)

add_subdirectory(Builder)
add_subdirectory(Flood)
add_subdirectory(Launch)

add_executable(malformed_datagrams main.cpp)

target_link_libraries(malformed_datagrams
  PRIVATE
    rtype_tools_malformed_datagrams_flood
    rtype_tools_malformed_datagrams_launch
)

set_target_properties(malformed_datagrams PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

rtype_enable_warnings(malformed_datagrams)

find_package(libhydrogen CONFIG REQUIRED CONFIGS hydrogen-config.cmake)

add_library(rtype_master_servers_helpers STATIC
  ServerKey.cpp
)

target_include_directories(rtype_master_servers_helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_servers_helpers
  PRIVATE
    hydrogen::hydrogen
    rtype_network_master_validators
)

rtype_enable_warnings(rtype_master_servers_helpers)

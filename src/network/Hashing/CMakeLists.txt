find_package(libhydrogen CONFIG REQUIRED CONFIGS hydrogen-config.cmake)

add_library(rtype_network_hashing STATIC Hashing.cpp)

target_include_directories(rtype_network_hashing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_hashing
  PUBLIC
    rtype_network_constants
  PRIVATE
    hydrogen::hydrogen
)

rtype_enable_warnings(rtype_network_hashing)

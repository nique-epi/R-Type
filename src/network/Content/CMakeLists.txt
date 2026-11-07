find_package(libhydrogen CONFIG REQUIRED CONFIGS hydrogen-config.cmake)

add_library(rtype_network_content STATIC ContentFingerprintText.cpp ContentPack.cpp)

target_include_directories(rtype_network_content PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_content
  PUBLIC
    rtype_identifiers
    rtype_network_constants
    rtype_network_exceptions
  PRIVATE
    hydrogen::hydrogen
    rtype_network_serialization
)

rtype_enable_warnings(rtype_network_content)

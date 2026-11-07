add_library(rtype_network_content STATIC ContentFingerprintText.cpp ContentPack.cpp ContentPackRoute.cpp)

target_include_directories(rtype_network_content PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_content
  PUBLIC
    rtype_identifiers
    rtype_network_constants
    rtype_network_byte_text
    rtype_network_exceptions
    rtype_network_master_routes
  PRIVATE
    rtype_network_hashing
    rtype_network_serialization
)

rtype_enable_warnings(rtype_network_content)

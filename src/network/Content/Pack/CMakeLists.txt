add_library(rtype_network_content_pack STATIC ContentFingerprintText.cpp ContentPack.cpp)

target_include_directories(rtype_network_content_pack PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_content_pack
  PUBLIC
    rtype_identifiers
    rtype_network_constants
    rtype_network_byte_text
    rtype_network_exceptions
  PRIVATE
    rtype_network_hashing
    rtype_network_serialization
)

rtype_enable_warnings(rtype_network_content_pack)

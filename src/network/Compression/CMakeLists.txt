add_library(rtype_network_compression_constants INTERFACE)

target_include_directories(rtype_network_compression_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_compression_constants
  INTERFACE
    rtype_network_constants
    rtype_network_protocol
)

add_subdirectory(Zstd)

add_library(rtype_network_compression INTERFACE)

target_link_libraries(rtype_network_compression
  INTERFACE
    rtype_network_compression_constants
    rtype_network_compression_zstd
)

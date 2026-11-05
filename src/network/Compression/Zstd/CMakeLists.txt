find_package(zstd CONFIG REQUIRED)

add_library(rtype_network_compression_zstd STATIC ZstdCodec.cpp)

target_include_directories(rtype_network_compression_zstd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_compression_zstd
  PUBLIC
    rtype_network_compression_constants
  PRIVATE
    rtype_network_exceptions
    zstd::libzstd
)

rtype_enable_warnings(rtype_network_compression_zstd)

find_package(zstd CONFIG REQUIRED)

# The dictionary is an asset of the repository, compiled into both programs so
# they never hold two versions of it.
set(RTYPE_DICTIONARY_FILE ${CMAKE_SOURCE_DIR}/assets/compression/server_to_client.dict)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${RTYPE_DICTIONARY_FILE})
file(READ ${RTYPE_DICTIONARY_FILE} RTYPE_DICTIONARY_HEX HEX)
string(LENGTH "${RTYPE_DICTIONARY_HEX}" RTYPE_DICTIONARY_HEX_LENGTH)
math(EXPR RTYPE_DICTIONARY_SIZE "${RTYPE_DICTIONARY_HEX_LENGTH} / 2")
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " RTYPE_DICTIONARY_BYTES "${RTYPE_DICTIONARY_HEX}")
configure_file(ServerToClientDictionary.cpp.in ServerToClientDictionary.cpp @ONLY)

add_library(rtype_network_compression_zstd STATIC
  ZstdCodec.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/ServerToClientDictionary.cpp
)

target_include_directories(rtype_network_compression_zstd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_compression_zstd
  PUBLIC
    rtype_network_compression_constants
  PRIVATE
    rtype_network_exceptions
    zstd::libzstd
)

rtype_enable_warnings(rtype_network_compression_zstd)

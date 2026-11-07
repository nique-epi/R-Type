add_subdirectory(Download)
add_subdirectory(Cache)
add_subdirectory(Reading)

add_library(rtype_client_content_pack STATIC ContentLoading.cpp)

target_include_directories(rtype_client_content_pack PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_content_pack
  PUBLIC
    rtype_client_content_pack_cache
    rtype_client_content_pack_download
    rtype_client_content_pack_reading
    rtype_identifiers
    rtype_network_content
  PRIVATE
    rtype_network_constants
)

rtype_enable_warnings(rtype_client_content_pack)

add_library(rtype_client_content_pack_cache STATIC ContentPackCache.cpp)

target_include_directories(rtype_client_content_pack_cache PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_content_pack_cache
  PUBLIC
    rtype_identifiers
    rtype_network_content
  PRIVATE
    rtype_engine_data_files
    rtype_engine_exceptions
)

rtype_enable_warnings(rtype_client_content_pack_cache)

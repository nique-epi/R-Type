find_package(httplib CONFIG REQUIRED)

add_library(rtype_client_content_pack STATIC
  ContentLoading.cpp
  ContentPackCache.cpp
  ContentPackDownload.cpp
  PackContentReading.cpp
)

target_include_directories(rtype_client_content_pack PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_content_pack
  PUBLIC
    rtype_client_entity_appearances
    rtype_game_entity_types
    rtype_identifiers
    rtype_network_content
  PRIVATE
    httplib::httplib
    rtype_client_exceptions
    rtype_engine_data_files
    rtype_network_exceptions
)

rtype_enable_warnings(rtype_client_content_pack)

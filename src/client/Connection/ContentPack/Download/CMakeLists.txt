add_library(rtype_client_content_pack_download STATIC ContentPackDownload.cpp)

target_include_directories(rtype_client_content_pack_download PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_content_pack_download
  PRIVATE
    httplib::httplib
    rtype_network_byte_text
    rtype_network_http_api
)

rtype_enable_warnings(rtype_client_content_pack_download)

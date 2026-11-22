add_library(rtype_client_reported_textures STATIC ReportedTextures.cpp)

target_include_directories(rtype_client_reported_textures PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_reported_textures
  PUBLIC
    rtype_client_asset_texture_source
    rtype_logging
  PRIVATE
    rtype_client_interface
)

rtype_enable_warnings(rtype_client_reported_textures)

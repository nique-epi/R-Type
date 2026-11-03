add_library(rtype_client_asset_texture_source STATIC AssetTextureSource.cpp)

target_include_directories(rtype_client_asset_texture_source PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_asset_texture_source
  PUBLIC
    rtype_client_sprites
    rtype_client_assets_module
)

rtype_enable_warnings(rtype_client_asset_texture_source)

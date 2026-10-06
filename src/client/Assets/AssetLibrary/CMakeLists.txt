add_library(rtype_client_asset_library STATIC AssetLibrary.cpp)

target_include_directories(rtype_client_asset_library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_asset_library
  PUBLIC
    rtype_client_assets
    SFML::Graphics
    SFML::Audio
)

rtype_enable_warnings(rtype_client_asset_library)

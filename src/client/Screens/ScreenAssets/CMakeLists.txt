add_library(rtype_client_screen_assets STATIC ScreenAssets.cpp)

target_include_directories(rtype_client_screen_assets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_screen_assets
  PUBLIC
    rtype_client_assets
    rtype_client_screen_interfaces
    rtype_client_widgets
    SFML::Graphics
  PRIVATE
    rtype_client_asset_loading_step
    rtype_client_audio_interfaces
    rtype_client_effect_interfaces
    rtype_client_interface
)

rtype_enable_warnings(rtype_client_screen_assets)

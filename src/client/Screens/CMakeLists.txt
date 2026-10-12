add_library(rtype_client_screens STATIC ScreenAssets.cpp)

target_include_directories(rtype_client_screens PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_screens
  PUBLIC
    rtype_client_assets
    SFML::Graphics
  PRIVATE
    rtype_client_asset_loading_step
)

rtype_enable_warnings(rtype_client_screens)

add_subdirectory(GameScreen)
add_subdirectory(LoadingScreen)
add_subdirectory(OptionsScreen)

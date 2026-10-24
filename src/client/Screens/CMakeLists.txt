add_library(rtype_client_screen_interfaces INTERFACE)

target_include_directories(rtype_client_screen_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_screen_interfaces INTERFACE rtype_engine_time)

add_library(rtype_client_screens STATIC ScreenAssets.cpp)

target_link_libraries(rtype_client_screens
  PUBLIC
    rtype_client_assets
    rtype_client_screen_interfaces
    rtype_client_widgets
    SFML::Graphics
  PRIVATE
    rtype_client_asset_loading_step
    rtype_client_effect_interfaces
    rtype_client_interface
)

rtype_enable_warnings(rtype_client_screens)

add_subdirectory(ConnectionScreen)
add_subdirectory(GameScreen)
add_subdirectory(LoadingScreen)
add_subdirectory(LobbyScreen)
add_subdirectory(OptionsScreen)
add_subdirectory(TitleScreen)
add_subdirectory(QuitConfirmationScreen)

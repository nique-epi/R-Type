add_library(rtype_client_screen_interfaces INTERFACE)

target_include_directories(rtype_client_screen_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_screen_interfaces
  INTERFACE
    rtype_client_application
    rtype_client_audio_contract
    rtype_engine_time
    rtype_network_transport
)

add_subdirectory(ScreenAssets)
add_subdirectory(PlayerPreferences)
add_subdirectory(ConnectionScreen)
add_subdirectory(IntroScreen)
add_subdirectory(LoadingScreen)
add_subdirectory(LobbyScreen)
add_subdirectory(OptionsRows)
add_subdirectory(OptionsDescriptions)
add_subdirectory(OptionsTabs)
add_subdirectory(SavedStamp)
add_subdirectory(ResetDialog)
add_subdirectory(OptionsScreen)
add_subdirectory(TitleScreen)
add_subdirectory(DialogScreen)
add_subdirectory(DialogMenus)

add_library(rtype_client_intro_interfaces INTERFACE)

target_include_directories(rtype_client_intro_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_intro_interfaces
  INTERFACE
    rtype_client_audio_interfaces
    rtype_client_effect_interfaces
    rtype_client_interface
    rtype_client_mission_start_interfaces
)

add_subdirectory(IntroHash)
add_subdirectory(IntroTimeline)
add_subdirectory(IntroShipPaths)
add_subdirectory(IntroMobPaths)
add_subdirectory(IntroCastBuilder)
add_subdirectory(IntroCues)
add_subdirectory(CinemaProjection)
add_subdirectory(IntroFleetPainting)
add_subdirectory(IntroFirePainting)
add_subdirectory(IntroBoostPainting)
add_subdirectory(IntroPainting)

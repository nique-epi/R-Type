add_library(rtype_client_sky_events INTERFACE)

target_include_directories(rtype_client_sky_events INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_sky_events
  INTERFACE
    rtype_client_background
    rtype_client_tail_gradient
)

add_subdirectory(TailGradient)
add_subdirectory(ShootingStar)
add_subdirectory(Flare)
add_subdirectory(Asteroid)
add_subdirectory(Comet)
add_subdirectory(SkyEventScheduler)

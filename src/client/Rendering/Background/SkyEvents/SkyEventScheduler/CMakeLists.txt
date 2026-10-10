add_library(rtype_client_sky_event_scheduler STATIC SkyEventScheduler.cpp)

target_include_directories(rtype_client_sky_event_scheduler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_sky_event_scheduler
  PUBLIC
    rtype_client_sky_events
  PRIVATE
    rtype_client_asteroid
    rtype_client_comet
    rtype_client_flare
    rtype_client_random_range
    rtype_client_shooting_star
)

rtype_enable_warnings(rtype_client_sky_event_scheduler)

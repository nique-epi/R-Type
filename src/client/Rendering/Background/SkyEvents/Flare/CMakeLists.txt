add_library(rtype_client_flare STATIC Flare.cpp)

target_include_directories(rtype_client_flare PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_flare
  PUBLIC
    rtype_client_sky_events
  PRIVATE
    rtype_client_random_range
)

rtype_enable_warnings(rtype_client_flare)

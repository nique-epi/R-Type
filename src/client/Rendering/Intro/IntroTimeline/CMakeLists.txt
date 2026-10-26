add_library(rtype_client_intro_timeline STATIC IntroTimeline.cpp)

target_include_directories(rtype_client_intro_timeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_intro_timeline
  PUBLIC
    rtype_client_intro_interfaces
  PRIVATE
    rtype_client_easing
    rtype_client_pixel_rounding
)

rtype_enable_warnings(rtype_client_intro_timeline)

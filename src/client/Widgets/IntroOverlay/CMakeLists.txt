add_library(rtype_client_intro_overlay STATIC IntroOverlay.cpp)

target_include_directories(rtype_client_intro_overlay PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_intro_overlay
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_client_easing
    rtype_client_intro_timeline
)

rtype_enable_warnings(rtype_client_intro_overlay)

add_library(rtype_client_screen_off STATIC ScreenOff.cpp)

target_include_directories(rtype_client_screen_off PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_screen_off
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_client_easing
    rtype_client_effect_interfaces
)

rtype_enable_warnings(rtype_client_screen_off)

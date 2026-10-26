add_library(rtype_client_color_ramp STATIC ColorRamp.cpp)

target_include_directories(rtype_client_color_ramp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_color_ramp
  PUBLIC
    rtype_client_rendering_interfaces
    SFML::Graphics
)

rtype_enable_warnings(rtype_client_color_ramp)

add_library(rtype_client_edge_pulse STATIC EdgePulse.cpp)

target_include_directories(rtype_client_edge_pulse PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_edge_pulse
  PUBLIC
    rtype_client_rendering_interfaces
    rtype_client_widgets
  PRIVATE
    rtype_client_ground_effect_interfaces
)

rtype_enable_warnings(rtype_client_edge_pulse)

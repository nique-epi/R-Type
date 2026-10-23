add_library(rtype_client_gain_attribution STATIC GainAttribution.cpp)

target_include_directories(rtype_client_gain_attribution PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_gain_attribution
  PUBLIC
    rtype_client_effect_interfaces
    rtype_engine_time
  PRIVATE
    rtype_client_pilot_color
)

rtype_enable_warnings(rtype_client_gain_attribution)

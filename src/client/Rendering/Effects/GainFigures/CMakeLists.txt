add_library(rtype_client_gain_figures STATIC GainFigures.cpp)

target_include_directories(rtype_client_gain_figures PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_gain_figures
  PUBLIC
    rtype_client_effect_interfaces
    rtype_client_gain_attribution
    rtype_engine
  PRIVATE
    rtype_client_effect_targets
    rtype_client_sprite_effect_interfaces
)

rtype_enable_warnings(rtype_client_gain_figures)

add_library(rtype_client_effect_targets STATIC EffectTargets.cpp)

target_include_directories(rtype_client_effect_targets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_effect_targets
  PUBLIC
    rtype_client_effect_interfaces
    rtype_engine
  PRIVATE
    rtype_client_pilot_color
    rtype_client_player_number_label
)

rtype_enable_warnings(rtype_client_effect_targets)

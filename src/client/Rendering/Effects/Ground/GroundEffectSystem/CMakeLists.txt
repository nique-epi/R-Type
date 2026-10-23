add_library(rtype_client_ground_effect_system STATIC GroundEffectSystem.cpp)

target_include_directories(rtype_client_ground_effect_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_ground_effect_system
  PUBLIC
    rtype_client_ground_effect_interfaces
    rtype_engine
)

rtype_enable_warnings(rtype_client_ground_effect_system)

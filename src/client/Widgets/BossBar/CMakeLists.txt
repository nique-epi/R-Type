add_library(rtype_client_boss_bar STATIC BossBar.cpp)

target_include_directories(rtype_client_boss_bar PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_boss_bar
  PUBLIC
    rtype_client_boss
    rtype_client_rendering_interfaces
    rtype_client_widgets
  PRIVATE
    rtype_client_effect_interfaces
    rtype_network_boss_status
)

rtype_enable_warnings(rtype_client_boss_bar)

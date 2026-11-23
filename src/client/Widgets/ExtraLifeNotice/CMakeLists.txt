add_library(rtype_client_extra_life_notice STATIC ExtraLifeNotice.cpp)

target_include_directories(rtype_client_extra_life_notice PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_extra_life_notice
  PUBLIC
    rtype_client_widgets
    rtype_network_game_event
  PRIVATE
    rtype_client_pilot_color
    rtype_client_status_band
)

rtype_enable_warnings(rtype_client_extra_life_notice)

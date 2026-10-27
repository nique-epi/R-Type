add_library(rtype_client_taunt_label STATIC TauntLabel.cpp)

target_include_directories(rtype_client_taunt_label PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_taunt_label
  PUBLIC
    rtype_client_flight_log_interfaces
  PRIVATE
    rtype_client_easing
    rtype_client_entity_sprites
    rtype_client_flight_log_timeline
    rtype_client_typing
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_taunt_label)

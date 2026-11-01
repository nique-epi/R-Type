add_library(rtype_client_departure_feed STATIC DepartureFeed.cpp)

target_include_directories(rtype_client_departure_feed PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_departure_feed
  PUBLIC
    rtype_client_interface
    rtype_client_replication_interfaces
    rtype_client_widgets
  PRIVATE
    rtype_client_entity_sprites
    rtype_client_pilot_color
    rtype_client_pilot_name
    rtype_client_typing
)

rtype_enable_warnings(rtype_client_departure_feed)

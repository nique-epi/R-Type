add_library(rtype_client_shop_ticket STATIC
  PurchaseTicket.cpp
  TicketPainting.cpp
)

target_include_directories(rtype_client_shop_ticket PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_shop_ticket
  PUBLIC
    SFML::Window
    rtype_client_audio_contract_sounds
    rtype_client_interface
    rtype_client_window_display
    rtype_engine_time
    rtype_identifiers
  PRIVATE
    rtype_client_economy
    rtype_client_entity_sprites
    rtype_client_interface_pixel
    rtype_client_ship_hulls
    rtype_client_ship_looks
    rtype_client_widgets
)

rtype_enable_warnings(rtype_client_shop_ticket)

add_library(rtype_client_connection_screen STATIC ConnectionScreen.cpp)

target_include_directories(rtype_client_connection_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_connection_screen
  PUBLIC
    rtype_client_connection
    rtype_client_menu
    rtype_client_rendering_interfaces
    rtype_client_screens
    rtype_client_window
    rtype_network_constants
    rtype_network_protocol
    SFML::Graphics
  PRIVATE
    rtype_client_menu_builder
    rtype_client_text_field
    rtype_network_transport
    rtype_network_wire_types
)

rtype_enable_warnings(rtype_client_connection_screen)

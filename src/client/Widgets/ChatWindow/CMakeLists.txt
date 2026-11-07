add_library(rtype_client_chat_window STATIC ChatPainting.cpp ChatWindow.cpp)

target_include_directories(rtype_client_chat_window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_chat_window
  PUBLIC
    SFML::Window
    rtype_client_connection_chat
    rtype_client_interface
    rtype_client_settings_values
    rtype_client_text_field
    rtype_client_widgets
  PRIVATE
    rtype_client_input_keys
    rtype_client_pilot_color
    rtype_client_pilot_name
    rtype_client_sunk_box
    rtype_client_typing
    rtype_client_widget_text
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_chat_window)

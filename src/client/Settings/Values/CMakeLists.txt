add_library(rtype_client_settings_values INTERFACE)

target_include_directories(rtype_client_settings_values INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_settings_values
  INTERFACE
    rtype_client_input_keys
    rtype_network_transport
)

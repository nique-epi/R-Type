add_library(rtype_client_input_keys STATIC
  KeyBindings.cpp
  KeyNames.cpp
  LocalizedKeyName.cpp
)

target_include_directories(rtype_client_input_keys PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_input_keys
  PUBLIC
    SFML::Window
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_input_keys)

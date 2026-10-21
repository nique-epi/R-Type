add_library(rtype_client_input INTERFACE)

target_include_directories(rtype_client_input INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_input INTERFACE SFML::Window rtype_network_protocol)

add_library(rtype_client_key_names STATIC KeyNames.cpp)

target_link_libraries(rtype_client_key_names PUBLIC rtype_client_input)

rtype_enable_warnings(rtype_client_key_names)

add_library(rtype_client_localized_key_name STATIC LocalizedKeyName.cpp)

target_link_libraries(rtype_client_localized_key_name
  PUBLIC
    rtype_client_input
  PRIVATE
    rtype_client_key_names
)

rtype_enable_warnings(rtype_client_localized_key_name)

add_subdirectory(ActionSet)
add_subdirectory(KeyBindings)
add_subdirectory(PlayerCommands)

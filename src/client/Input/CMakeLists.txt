add_library(rtype_client_input INTERFACE)

target_include_directories(rtype_client_input INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(ActionSet)
add_subdirectory(KeyBindings)
add_subdirectory(PlayerCommands)

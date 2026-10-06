add_library(rtype_client_window INTERFACE)

target_include_directories(rtype_client_window INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(PlayfieldViewport)
add_subdirectory(GameWindow)

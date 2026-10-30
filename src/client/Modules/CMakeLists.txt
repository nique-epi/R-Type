add_library(rtype_client_module_constants INTERFACE)

target_include_directories(rtype_client_module_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Logger)
add_subdirectory(Assets)
add_subdirectory(Settings)
add_subdirectory(Window)
add_subdirectory(Connection)
add_subdirectory(Audio)
add_subdirectory(Screens)
add_subdirectory(Assembly)

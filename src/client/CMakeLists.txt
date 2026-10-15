find_package(SFML 3 COMPONENTS Graphics Window System Audio CONFIG REQUIRED)

add_library(rtype_client_constants INTERFACE)

target_include_directories(rtype_client_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_constants INTERFACE rtype_network_constants)

add_subdirectory(Exceptions)
add_subdirectory(Platform)
add_subdirectory(Assets)
add_subdirectory(Input)
add_subdirectory(Rendering)
add_subdirectory(Replication)
add_subdirectory(Window)
add_subdirectory(Widgets)
add_subdirectory(Screens)
add_subdirectory(Application)
add_subdirectory(Connection)
add_subdirectory(LaunchOptions)

add_executable(r-type_client main.cpp)

target_link_libraries(r-type_client
  PRIVATE
    rtype_client_application
    rtype_client_constants
    rtype_client_launch_options
    rtype_client_rendering
    rtype_game
    rtype_logging
    rtype_network
)

rtype_enable_warnings(r-type_client)

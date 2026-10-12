find_package(SFML 3 COMPONENTS Graphics Window System Audio CONFIG REQUIRED)

add_subdirectory(Exceptions)
add_subdirectory(Platform)
add_subdirectory(Assets)
add_subdirectory(Rendering)
add_subdirectory(Window)
add_subdirectory(Screens)
add_subdirectory(Application)

add_executable(r-type_client main.cpp)

target_link_libraries(r-type_client
  PRIVATE
    rtype_client_application
    rtype_client_rendering
    rtype_game
    rtype_logging
    rtype_network
)

rtype_enable_warnings(r-type_client)

find_package(SFML 3 COMPONENTS Graphics Window System Audio CONFIG REQUIRED)

add_subdirectory(Window)

add_executable(r-type_client main.cpp)

target_link_libraries(r-type_client
  PRIVATE
    rtype_client_window
    rtype_game
    rtype_logging
    rtype_network
    SFML::Audio
)

rtype_enable_warnings(r-type_client)

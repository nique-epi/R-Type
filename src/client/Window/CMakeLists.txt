add_library(rtype_client_window STATIC GameWindow.cpp)

target_include_directories(rtype_client_window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_window PUBLIC SFML::Graphics)

rtype_enable_warnings(rtype_client_window)

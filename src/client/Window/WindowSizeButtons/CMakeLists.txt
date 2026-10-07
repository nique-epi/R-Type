add_library(rtype_client_window_size_buttons STATIC WindowSizeButtons.cpp)

target_include_directories(rtype_client_window_size_buttons PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_window_size_buttons
  PUBLIC
    SFML::Graphics
    rtype_client_window_size_selection
  PRIVATE
    rtype_client_window
    rtype_client_exceptions
)

rtype_enable_warnings(rtype_client_window_size_buttons)

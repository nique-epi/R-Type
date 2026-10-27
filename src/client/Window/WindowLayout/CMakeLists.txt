add_library(rtype_client_window_layout STATIC WindowLayout.cpp)

target_include_directories(rtype_client_window_layout PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_window_layout
  PUBLIC
    rtype_client_window
    rtype_client_window_size_selection
)

rtype_enable_warnings(rtype_client_window_layout)

add_library(rtype_client_row_scrolling STATIC RowScrolling.cpp)

target_include_directories(rtype_client_row_scrolling PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_row_scrolling
  PUBLIC
    rtype_client_widgets
    rtype_client_window_display
  PRIVATE
    rtype_client_interface_pixel
)

rtype_enable_warnings(rtype_client_row_scrolling)

add_library(rtype_client_scroll_mark STATIC ScrollMark.cpp)

target_include_directories(rtype_client_scroll_mark PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_scroll_mark
  PUBLIC
    rtype_client_row_window
    rtype_client_widgets
)

rtype_enable_warnings(rtype_client_scroll_mark)

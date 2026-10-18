add_library(rtype_client_switch STATIC Switch.cpp)

target_include_directories(rtype_client_switch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_switch
  PUBLIC
    rtype_client_row_control
  PRIVATE
    rtype_client_sunk_box
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_switch)

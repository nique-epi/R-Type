add_library(rtype_client_dropdown STATIC Dropdown.cpp)

target_include_directories(rtype_client_dropdown PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_dropdown
  PUBLIC
    rtype_client_row_control
  PRIVATE
    rtype_client_chamfered_box
    rtype_client_check_box
    rtype_client_sunk_box
)

rtype_enable_warnings(rtype_client_dropdown)

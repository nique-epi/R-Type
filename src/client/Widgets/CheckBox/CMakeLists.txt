add_library(rtype_client_check_box STATIC CheckBox.cpp)

target_include_directories(rtype_client_check_box PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_check_box
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_client_outline
)

rtype_enable_warnings(rtype_client_check_box)

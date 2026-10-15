add_library(rtype_client_button STATIC Button.cpp)

target_include_directories(rtype_client_button PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_button
  PUBLIC
    rtype_client_chamfered_box
    rtype_client_widgets
)

rtype_enable_warnings(rtype_client_button)

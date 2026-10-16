add_library(rtype_client_text_field STATIC TextField.cpp)

target_include_directories(rtype_client_text_field PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_text_field
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_network_constants
)

rtype_enable_warnings(rtype_client_text_field)

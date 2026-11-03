add_library(rtype_client_ink_tag STATIC InkTag.cpp)

target_include_directories(rtype_client_ink_tag PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_ink_tag
  PUBLIC
    rtype_client_widgets
)

rtype_enable_warnings(rtype_client_ink_tag)

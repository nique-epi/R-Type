add_library(rtype_client_key_cap STATIC KeyCap.cpp)

target_include_directories(rtype_client_key_cap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_key_cap
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_key_cap)

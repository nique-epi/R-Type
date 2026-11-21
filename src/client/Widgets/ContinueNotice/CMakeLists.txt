add_library(rtype_client_continue_notice STATIC ContinueNotice.cpp)

target_include_directories(rtype_client_continue_notice PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_continue_notice
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_client_ground_effect_interfaces
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_continue_notice)

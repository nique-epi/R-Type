add_library(rtype_client_tab_rail STATIC TabRail.cpp)

target_include_directories(rtype_client_tab_rail PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_tab_rail
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_tab_rail)

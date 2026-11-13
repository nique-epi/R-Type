add_library(rtype_client_panel_layouts STATIC PanelLayouts.cpp)

target_include_directories(rtype_client_panel_layouts PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_panel_layouts
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_client_text_measures
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_panel_layouts)

add_library(rtype_client_widgets INTERFACE)

target_include_directories(rtype_client_widgets INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_widgets
  INTERFACE
    rtype_client_rendering_interfaces
    rtype_engine_time
    SFML::Graphics
    SFML::Window
)

add_library(rtype_client_widget_text STATIC WidgetText.cpp)

target_link_libraries(rtype_client_widget_text PUBLIC rtype_client_widgets)

rtype_enable_warnings(rtype_client_widget_text)

add_subdirectory(ChamferedBox)
add_subdirectory(Outline)
add_subdirectory(SunkBox)
add_subdirectory(CheckBox)
add_subdirectory(KeyCap)
add_subdirectory(TabStrip)
add_subdirectory(Label)
add_subdirectory(TextField)
add_subdirectory(Button)
add_subdirectory(Menu)
add_subdirectory(MenuBuilder)

add_library(rtype_client_widgets INTERFACE)

target_include_directories(rtype_client_widgets INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_widgets
  INTERFACE
    rtype_client_rendering_interfaces
    rtype_engine_time
    SFML::Graphics
    SFML::Window
)

add_subdirectory(ChamferedBox)
add_subdirectory(Label)
add_subdirectory(TextField)
add_subdirectory(Button)
add_subdirectory(Menu)
add_subdirectory(MenuBuilder)

add_library(rtype_client_widgets STATIC ChamferedBox.cpp)

target_include_directories(rtype_client_widgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_widgets
  PUBLIC
    rtype_client_rendering_interfaces
    rtype_engine_time
    SFML::Graphics
    SFML::Window
)

rtype_enable_warnings(rtype_client_widgets)

add_subdirectory(Label)
add_subdirectory(Button)
add_subdirectory(Menu)
add_subdirectory(MenuBuilder)

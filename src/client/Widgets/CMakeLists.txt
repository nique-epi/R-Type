add_library(rtype_client_widgets INTERFACE)

target_include_directories(rtype_client_widgets INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_widgets
  INTERFACE
    rtype_client_interface
    rtype_engine_time
    SFML::Graphics
    SFML::Window
)

add_library(rtype_client_widget_text STATIC WidgetText.cpp)

target_link_libraries(rtype_client_widget_text PUBLIC rtype_client_widgets)

rtype_enable_warnings(rtype_client_widget_text)

add_library(rtype_client_window_size_chart STATIC WindowSizeChart.cpp)

target_link_libraries(rtype_client_window_size_chart
  PUBLIC
    rtype_client_widgets
    rtype_client_window_size_selection
  PRIVATE
    rtype_client_outline
    rtype_client_window_size_text
)

rtype_enable_warnings(rtype_client_window_size_chart)

add_library(rtype_client_hangar_bay STATIC HangarBay.cpp)

target_link_libraries(rtype_client_hangar_bay
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_client_cut_corner_box
    rtype_client_entity_sprites
    rtype_client_sunk_box
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_hangar_bay)

add_library(rtype_client_status_band STATIC StatusBand.cpp)

target_link_libraries(rtype_client_status_band
  PUBLIC
    rtype_client_replication_interfaces
    rtype_client_widgets
  PRIVATE
    rtype_client_cut_corner_box
    rtype_client_entity_sprites
    rtype_client_outline
    rtype_client_widget_text
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_status_band)

add_library(rtype_client_mission_banner STATIC MissionBanner.cpp)

target_link_libraries(rtype_client_mission_banner
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_mission_banner)

add_subdirectory(CutCornerBox)
add_subdirectory(Outline)
add_subdirectory(SunkBox)
add_subdirectory(CheckBox)
add_subdirectory(KeyCap)
add_subdirectory(TabStrip)
add_subdirectory(Label)
add_subdirectory(TextField)
add_subdirectory(RowControls)
add_subdirectory(Button)
add_subdirectory(Menu)
add_subdirectory(MenuBuilder)

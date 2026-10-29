add_library(rtype_client_widgets INTERFACE)

target_include_directories(rtype_client_widgets INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_widgets
  INTERFACE
    rtype_client_interface
    rtype_engine_time
    SFML::Graphics
    SFML::Window
)

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

add_subdirectory(HangarBay)
add_subdirectory(PaddedScore)
add_subdirectory(PilotName)
add_subdirectory(WidgetText)
add_subdirectory(StatusBand)
add_subdirectory(MissionBanner)
add_subdirectory(IntroOverlay)
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
add_subdirectory(HelpWalkthrough)
add_subdirectory(FlightLog)

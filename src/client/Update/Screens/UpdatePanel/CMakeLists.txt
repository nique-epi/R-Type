add_library(rtype_client_update_panel STATIC
  UpdateScreen.cpp
)

target_include_directories(rtype_client_update_panel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_update_panel
  PUBLIC
    rtype_client_application
    rtype_client_interface
    rtype_client_widgets
    rtype_engine_time
    rtype_client_update_run
  PRIVATE
    rtype_client_cut_corner_box
    rtype_client_game_module
)

rtype_enable_warnings(rtype_client_update_panel)

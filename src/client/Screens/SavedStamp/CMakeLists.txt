add_library(rtype_client_saved_stamp STATIC SavedStamp.cpp)

target_include_directories(rtype_client_saved_stamp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_saved_stamp
  PUBLIC
    rtype_client_key_bindings
    rtype_client_settings
    rtype_client_slider
    rtype_client_window
    rtype_client_screen_assets
    rtype_engine_time
)

rtype_enable_warnings(rtype_client_saved_stamp)

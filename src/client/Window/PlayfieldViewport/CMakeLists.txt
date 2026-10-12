add_library(rtype_client_playfield_viewport STATIC PlayfieldViewport.cpp)

target_include_directories(rtype_client_playfield_viewport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_playfield_viewport
  PRIVATE
    rtype_client_window
    rtype_game
)

rtype_enable_warnings(rtype_client_playfield_viewport)

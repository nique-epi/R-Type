add_library(rtype_client_frame_picture STATIC FramePicture.cpp)

target_include_directories(rtype_client_frame_picture PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_frame_picture
  PUBLIC
    SFML::Graphics
  PRIVATE
    rtype_client_exceptions
    rtype_client_screen_stack
    rtype_client_scrolling_background
    rtype_game_components
)

rtype_enable_warnings(rtype_client_frame_picture)

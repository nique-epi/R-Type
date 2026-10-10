add_library(rtype_client_loading_screen STATIC LoadingScreen.cpp)

target_include_directories(rtype_client_loading_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_loading_screen
  PUBLIC
    rtype_client_asset_loading_step
    rtype_client_screen_stack
    SFML::Graphics
  PRIVATE
    rtype_client_screens
    rtype_game
)

rtype_enable_warnings(rtype_client_loading_screen)

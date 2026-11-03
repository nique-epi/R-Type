add_library(rtype_client_loading_screen STATIC LoadingScreen.cpp)

target_include_directories(rtype_client_loading_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_loading_screen
  PUBLIC
    rtype_client_assets_module
    rtype_client_screen_assets
    SFML::Graphics
  PRIVATE
    rtype_game
)

rtype_enable_warnings(rtype_client_loading_screen)

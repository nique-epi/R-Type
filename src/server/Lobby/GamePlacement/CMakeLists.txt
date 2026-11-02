add_library(rtype_server_game_placement STATIC GamePlacement.cpp)

target_include_directories(rtype_server_game_placement PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_game_placement
  PUBLIC
    rtype_server_instances
    rtype_server_lobby_constants
)

rtype_enable_warnings(rtype_server_game_placement)

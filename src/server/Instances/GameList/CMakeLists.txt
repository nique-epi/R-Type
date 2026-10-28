add_library(rtype_server_game_list STATIC GameList.cpp)

target_include_directories(rtype_server_game_list PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_game_list
  PUBLIC
    rtype_server_exceptions
    rtype_server_game_instance
)

rtype_enable_warnings(rtype_server_game_list)

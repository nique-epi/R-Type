add_library(rtype_server_game_modules STATIC GameModules.cpp)

target_include_directories(rtype_server_game_modules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_game_modules
  PUBLIC
    rtype_engine
    rtype_server_instances
)

rtype_enable_warnings(rtype_server_game_modules)

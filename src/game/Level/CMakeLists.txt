add_library(rtype_game_level STATIC LevelScroll.cpp)

target_include_directories(rtype_game_level PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_level PUBLIC rtype_engine_time)

rtype_enable_warnings(rtype_game_level)

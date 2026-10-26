add_library(rtype_game_waves INTERFACE)

target_include_directories(rtype_game_waves INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_waves INTERFACE rtype_engine_time)

add_subdirectory(WaveProgression)

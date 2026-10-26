add_library(rtype_game_wave_director STATIC WaveDirector.cpp)

target_include_directories(rtype_game_wave_director PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_wave_director
  PUBLIC
    rtype_engine_timer_scheduler
    rtype_game_waves
  PRIVATE
    rtype_game_exceptions
    rtype_game_wave_progression
)

rtype_enable_warnings(rtype_game_wave_director)

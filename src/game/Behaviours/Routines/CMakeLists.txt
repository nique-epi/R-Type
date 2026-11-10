add_library(rtype_game_behaviours_routines STATIC
  Routine.cpp
)

target_include_directories(rtype_game_behaviours_routines PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_behaviours_routines
  PUBLIC
    rtype_game_components
)

rtype_enable_warnings(rtype_game_behaviours_routines)

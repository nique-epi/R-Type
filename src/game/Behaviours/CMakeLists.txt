add_subdirectory(Piloting)
add_subdirectory(Rtype)

add_library(rtype_game_behaviours INTERFACE)

target_link_libraries(rtype_game_behaviours
  INTERFACE
    rtype_game_behaviours_piloting
    rtype_game_behaviours_rtype
)

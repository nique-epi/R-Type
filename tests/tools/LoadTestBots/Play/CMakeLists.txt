add_subdirectory(Sight)
add_subdirectory(Pilot)
add_subdirectory(Tally)

add_library(rtype_tools_load_test_bots_play INTERFACE)

target_link_libraries(rtype_tools_load_test_bots_play
  INTERFACE
    rtype_tools_load_test_bots_play_pilot
    rtype_tools_load_test_bots_play_sight
    rtype_tools_load_test_bots_play_tally
)

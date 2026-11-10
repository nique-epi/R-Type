add_library(rtype_game_scripting STATIC
  BehaviourScriptFiles.cpp
  BehaviourScripts.cpp
  LuauBehaviourTables.cpp
  LuauBehaviour.cpp
  LuauBehaviourState.cpp
  LuauEventBindings.cpp
  LuauEventSubscription.cpp
)

target_include_directories(rtype_game_scripting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_scripting
  PUBLIC
    rtype_engine_event_bus
    rtype_engine_scripting
    rtype_game_behaviours
    rtype_game_behaviours_piloting
    rtype_game_components
    rtype_game_events
    rtype_game_spawning_constants
  PRIVATE
    rtype_engine_data_files
    rtype_engine_time
    rtype_logging
)

rtype_enable_warnings(rtype_game_scripting)

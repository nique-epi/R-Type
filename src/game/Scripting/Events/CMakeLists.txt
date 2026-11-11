add_library(rtype_game_scripting_events STATIC
  LuauEventBindings.cpp
  LuauEventSubscription.cpp
  LuauEventSubscriptions.cpp
)

target_include_directories(rtype_game_scripting_events PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_scripting_events
  PUBLIC
    rtype_engine_event_bus
    rtype_engine_scripting
    rtype_game_events
  PRIVATE
    rtype_game_scripting_constants
)

rtype_enable_warnings(rtype_game_scripting_events)

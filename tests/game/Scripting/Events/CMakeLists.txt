target_sources(game_scripting_tests PRIVATE
  LuauEventBindingsFixture.cpp
  LuauEventBindingsTest.cpp
  LuauEventSubscriptionTest.cpp
  LuauEventSubscriptionsTest.cpp
)

target_include_directories(game_scripting_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

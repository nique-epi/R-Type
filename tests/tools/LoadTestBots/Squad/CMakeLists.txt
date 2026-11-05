add_library(rtype_tools_load_test_bots_squad STATIC
  Bot.cpp
  BotSquad.cpp
  ReadyRule.cpp
)

target_include_directories(rtype_tools_load_test_bots_squad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_tools_load_test_bots_squad
  PUBLIC
    rtype_tools_load_test_bots_launch
    rtype_tools_load_test_bots_play
)

rtype_enable_warnings(rtype_tools_load_test_bots_squad)

rtype_add_test(NAME load_test_bots_squad_tests SOURCES ReadyRuleTest.cpp LIBS rtype_tools_load_test_bots_squad)

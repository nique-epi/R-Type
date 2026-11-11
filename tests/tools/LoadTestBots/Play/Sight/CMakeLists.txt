add_library(rtype_tools_load_test_bots_play_sight STATIC BotSight.cpp)

target_include_directories(rtype_tools_load_test_bots_play_sight PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_tools_load_test_bots_play_sight
  PUBLIC
    rtype_client_connection_module
    rtype_client_prediction
)

rtype_enable_warnings(rtype_tools_load_test_bots_play_sight)

rtype_add_test(NAME load_test_bots_play_sight_tests SOURCES BotSightTest.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/FixedEntityTypeTable.cpp LIBS rtype_tools_load_test_bots_play_sight)

target_include_directories(load_test_bots_play_sight_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests/doubles)

add_subdirectory(Fixtures)

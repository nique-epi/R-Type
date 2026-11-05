target_sources(engine_tests PRIVATE ServerClockTest.cpp ServerClockFixtures.cpp)
target_include_directories(engine_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

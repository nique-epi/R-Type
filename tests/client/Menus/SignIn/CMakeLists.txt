target_sources(client_screen_tests PRIVATE SignInFixtures.cpp SignInScreenFixture.cpp SignInScreenTest.cpp SignInTextTest.cpp)
target_include_directories(client_screen_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/client/Widgets)

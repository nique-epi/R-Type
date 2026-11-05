rtype_add_test(NAME client_audio_tests SOURCES ${RTYPE_TEST_SOURCE_DIR}/doubles/FakeMusicClock.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/FakeMusicPlayer.cpp LIBS rtype_client_audio_module)
target_include_directories(client_audio_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR}/client/Audio ${RTYPE_TEST_SOURCE_DIR}/client/Audio/Music ${RTYPE_TEST_SOURCE_DIR}/client/Audio/Sounds ${RTYPE_TEST_SOURCE_DIR}/client/Screens ${RTYPE_TEST_SOURCE_DIR}/client/Screens/Common ${RTYPE_TEST_SOURCE_DIR}/client/Screens/Registry ${RTYPE_TEST_SOURCE_DIR}/doubles)
target_sources(client_module_tests PRIVATE ClientAudioModuleTest.cpp ClientAudioModuleFixture.cpp)

add_subdirectory(Contract)
add_subdirectory(Music)
add_subdirectory(Sounds)

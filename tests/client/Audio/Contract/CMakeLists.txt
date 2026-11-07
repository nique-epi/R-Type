rtype_add_test(NAME client_null_audio_tests SOURCES ${RTYPE_TEST_SOURCE_DIR}/doubles/FakeMusicClock.cpp LIBS rtype_client_audio_contract)
target_include_directories(client_null_audio_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR}/client/Audio ${RTYPE_TEST_SOURCE_DIR}/client/Audio/Music ${RTYPE_TEST_SOURCE_DIR}/client/Audio/Sounds ${RTYPE_TEST_SOURCE_DIR}/client/Screens ${RTYPE_TEST_SOURCE_DIR}/doubles)

add_subdirectory(Music)
add_subdirectory(Service)
add_subdirectory(Sounds)

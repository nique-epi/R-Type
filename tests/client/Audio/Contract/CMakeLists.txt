rtype_add_test(NAME client_null_audio_tests LIBS rtype_client_audio_contract)
target_include_directories(client_null_audio_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR}/client/Audio ${RTYPE_TEST_SOURCE_DIR}/client/Audio/Music ${RTYPE_TEST_SOURCE_DIR}/client/Audio/Sounds)

add_subdirectory(Music)
add_subdirectory(Service)
add_subdirectory(Sounds)

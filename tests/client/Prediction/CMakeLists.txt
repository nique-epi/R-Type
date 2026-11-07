rtype_add_test(NAME client_prediction_tests SOURCES ${RTYPE_TEST_SOURCE_DIR}/client/Replication/Entities/EntityAnnouncements.cpp ${RTYPE_TEST_SOURCE_DIR}/client/Replication/Entities/EntityReplicatorFixtures.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingPlayerInputLink.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingSoundPlayer.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/FixedEntityTypeSource.cpp LIBS rtype_client_prediction rtype_client_sprite_effect_interfaces GTest::gmock)
target_include_directories(client_prediction_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR}/client/Rendering ${RTYPE_TEST_SOURCE_DIR}/client/Replication/Entities ${RTYPE_TEST_SOURCE_DIR}/doubles $<TARGET_PROPERTY:rtype_client_entity_sprites,INTERFACE_INCLUDE_DIRECTORIES>)

add_subdirectory(Interpolation)
add_subdirectory(OwnShip)

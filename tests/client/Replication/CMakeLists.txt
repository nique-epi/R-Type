rtype_add_test(NAME client_replication_tests SOURCES ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingSoundPlayer.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/FixedEntityTypeSource.cpp LIBS rtype_client_pilots rtype_client_sprite_effect_interfaces rtype_identifiers GTest::gmock)
target_include_directories(client_replication_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR}/client/Rendering ${RTYPE_TEST_SOURCE_DIR}/client/Replication/Entities ${RTYPE_TEST_SOURCE_DIR}/doubles $<TARGET_PROPERTY:rtype_client_entity_sprites,INTERFACE_INCLUDE_DIRECTORIES>)

add_subdirectory(Entities)
add_subdirectory(EntityMessages)
add_subdirectory(Pilots)

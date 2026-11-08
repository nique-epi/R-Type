target_sources(logging_tests PRIVATE LoggerJournalTest.cpp LoggerStandardErrorTest.cpp LoggerStandardErrorFixture.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingJournal.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingStandardError.cpp)
target_include_directories(logging_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR}/doubles)

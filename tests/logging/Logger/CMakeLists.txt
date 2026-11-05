target_sources(logging_tests PRIVATE LoggerJournalTest.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingJournal.cpp)
target_include_directories(logging_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR}/doubles)

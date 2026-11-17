find_package(spdlog CONFIG REQUIRED)

add_library(rtype_logging_journal_writer STATIC JournalSink.cpp JournalWriter.cpp)

target_include_directories(rtype_logging_journal_writer
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(rtype_logging_journal_writer
  PUBLIC
    rtype_logging_records
  PRIVATE
    spdlog::spdlog
)

rtype_enable_warnings(rtype_logging_journal_writer)

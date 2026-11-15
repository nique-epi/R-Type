find_package(Threads REQUIRED)

# The headers of the records a logger writes (LogLevel, LogRecord, LogEvent,
# LogFormat) and the constants of the library, without the logger itself: what
# the libraries the logger builds on read.
add_library(rtype_logging_records INTERFACE)

target_include_directories(rtype_logging_records
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_library(rtype_logging_logger STATIC Logger.cpp ProcessJournal.cpp)

target_include_directories(rtype_logging_logger
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(rtype_logging_logger
  PUBLIC
    Threads::Threads
    rtype_logging_exceptions
    rtype_logging_path_text
    rtype_logging_journal_writer
    rtype_logging_records
)

rtype_enable_warnings(rtype_logging_logger)

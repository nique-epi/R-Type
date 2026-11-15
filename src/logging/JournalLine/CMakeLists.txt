find_package(nlohmann_json CONFIG REQUIRED)

add_library(rtype_logging_journal_line STATIC JournalLine.cpp)

target_include_directories(rtype_logging_journal_line
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(rtype_logging_journal_line
  PUBLIC
    rtype_logging_records
  PRIVATE
    nlohmann_json::nlohmann_json
)

rtype_enable_warnings(rtype_logging_journal_line)

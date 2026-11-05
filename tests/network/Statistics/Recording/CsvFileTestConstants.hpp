#pragma once

#include <string_view>

/** @brief The file the tests of CsvFile write, the header they give it, and
 * the first line it must then hold. */
constexpr std::string_view CSV_FILE_NAME = "rows.csv";
constexpr std::string_view CSV_HEADER = "first,second";
constexpr std::string_view CSV_HEADER_LINE = "first,second\n";

/** @brief What a file held before a CSV file opened it, and a row appended to
 * it. */
constexpr std::string_view EARLIER_RUN_TEXT = "an earlier run\n";
constexpr std::string_view SOME_ROW = "a,row\n";

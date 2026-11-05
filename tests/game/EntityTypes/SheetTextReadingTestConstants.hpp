#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::game::testing {

/** @brief Files a folder of sheets may hold: two sheets, given out of the
 * order of their names, a file that is not a sheet, and a sheet whose name a
 * type may not have. */
constexpr std::string_view WEAVER_FILE_NAME = "weaver.toml";
constexpr std::string_view DRIFTER_FILE_NAME = "drifter.toml";
constexpr std::string_view UPPERCASE_FILE_NAME = "Drifter.toml";
constexpr std::string_view README_FILE_NAME = "README.md";
constexpr std::string_view README_TEXT = "Not a sheet.\n";
constexpr std::string_view SOME_SHEET_TEXT = "[gameplay]\n[appearance]\n";
constexpr std::string_view OTHER_SHEET_TEXT = "[appearance]\n[gameplay]\n";

/** @brief The names of the two sheets, in the order of the names. */
constexpr std::string_view FIRST_SHEET_NAME = "drifter";
constexpr std::string_view SECOND_SHEET_NAME = "weaver";
constexpr std::size_t SHEETS_IN_THE_FOLDER = 2;

/** @brief A folder of the temporary folder that does not exist. */
constexpr std::string_view MISSING_FOLDER_NAME = "no_such_folder";

/** @brief One file more than a program loads, with the extension of a sheet
 * and the first part of their names. */
constexpr std::size_t FILES_ONE_TOO_MANY = 33;
constexpr std::string_view GENERATED_FILE_PREFIX = "sheet_";
constexpr std::string_view SHEET_EXTENSION = ".toml";

}  // namespace rtype::game::testing

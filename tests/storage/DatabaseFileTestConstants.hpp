#pragma once

#include <array>
#include <string_view>

namespace rtype::storage::testing {

constexpr std::string_view DATABASE_FILE_NAME = "test.db";
constexpr std::string_view MEMORY_DATABASE = ":memory:";
constexpr std::string_view MISSING_FOLDER_FILE = "no/such/folder/test.db";

constexpr std::string_view CREATE_SCORES =
    "CREATE TABLE scores (score INTEGER NOT NULL) STRICT;";
constexpr std::string_view ADD_ONE_SCORE = "INSERT INTO scores VALUES (7);";
constexpr std::string_view ADD_A_SCORE_THEN_FAIL =
    "INSERT INTO scores VALUES (7); THIS IS NOT SQL;";
constexpr std::string_view COUNT_SCORES = "SELECT COUNT(*) FROM scores";

constexpr std::array<std::string_view, 1> FIRST_VERSION{CREATE_SCORES};
constexpr std::array<std::string_view, 2> SECOND_VERSION{CREATE_SCORES,
                                                         ADD_ONE_SCORE};
constexpr std::array<std::string_view, 2> SECOND_VERSION_FAILS{
    CREATE_SCORES, ADD_A_SCORE_THEN_FAIL};

constexpr std::string_view READ_JOURNAL_MODE = "PRAGMA journal_mode";
constexpr std::string_view READ_SYNCHRONOUS = "PRAGMA synchronous";
constexpr std::string_view READ_FOREIGN_KEYS = "PRAGMA foreign_keys";
constexpr std::string_view READ_USER_VERSION = "PRAGMA user_version";

/** @brief The value PRAGMA synchronous gives for FULL, on the SQLite page. */
constexpr int SYNCHRONOUS_FULL_VALUE = 2;

}  // namespace rtype::storage::testing

#pragma once

#include <string_view>

/** @brief The path a client asks the master whether it answers. */
constexpr std::string_view HEALTH_URL = "/api/health";

/** @brief The member of the answer of HEALTH_URL that holds the version. */
constexpr std::string_view VERSION_MEMBER_NAME = "version";

/** @brief The database file the tests give the master, in their folder. */
constexpr std::string_view TEST_DATABASE_FILE = "master.db";

/** @brief A folder no test creates, so a file in it cannot be opened. */
constexpr std::string_view MISSING_FOLDER = "no-such-folder";

/** @brief The route a feature module adds, and the path a client calls. */
constexpr std::string_view FEATURE_ROUTE = "/feature";
constexpr std::string_view FEATURE_URL = "/api/feature";

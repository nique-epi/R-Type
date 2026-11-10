#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The feed Velopack asks for on macOS, where the latest release
 * serves it, and what it holds. */
constexpr std::string_view FEED_NAME = "releases.osx.json";
constexpr std::string_view FEED_PATH =
    "/nique-epi/R-Type/releases/latest/download/releases.osx.json";
constexpr std::string_view FEED_TEXT =
    R"({"Assets":[{"Version":"1.1.0","FileName":"R-Type-1.1.0-osx-full.nupkg"}]})";

/** @brief A feed name that would leave the folder of the latest release, and
 * the path it would be read at. */
constexpr std::string_view LEAVING_FEED_NAME = "../releases.osx.json";
constexpr std::string_view LEAVING_FEED_PATH =
    "/nique-epi/R-Type/releases/latest/download/../releases.osx.json";

/** @brief A package of release 1.1.0, where its release serves it, and what
 * it holds. */
constexpr std::string_view PACKAGE_VERSION = "1.1.0";
constexpr std::string_view PACKAGE_FILE = "R-Type-1.1.0-osx-full.nupkg";
constexpr std::string_view PACKAGE_PATH =
    "/nique-epi/R-Type/releases/download/v1.1.0/R-Type-1.1.0-osx-full.nupkg";
constexpr std::string_view PACKAGE_CONTENT = "package";
constexpr std::uint64_t PACKAGE_SIZE = 7;
/** @brief A package name that would leave the folder of its release, and
 * the path it would be read at. */
constexpr std::string_view LEAVING_PACKAGE_FILE = "../../evil.nupkg";
constexpr std::string_view LEAVING_PACKAGE_PATH =
    "/nique-epi/R-Type/releases/download/v1.1.0/../../evil.nupkg";
/** @brief The package and the kind of package of the feeds. */
constexpr std::string_view PACKAGE_ID = "R-Type";
constexpr std::string_view FULL_PACKAGE = "Full";

/** @brief The share Velopack hears once the package is saved. */
constexpr std::int16_t WHOLE_PACKAGE = 100;

}  // namespace rtype::client::testing

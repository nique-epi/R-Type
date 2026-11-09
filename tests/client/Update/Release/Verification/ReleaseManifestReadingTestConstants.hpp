#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::client::testing {

/** @brief The parts of a manifest of 1.1.0, around its notes and its Linux
 * package. */
constexpr std::string_view MANIFEST_START = R"({"version":"1.1.0","notes":)";
constexpr std::string_view MANIFEST_PACKAGES = R"(,"packages":{"Linux":)";
constexpr std::string_view MANIFEST_END = "}}";

/** @brief One note, and the line it holds. */
constexpr std::string_view ONE_NOTE = R"(["Serve the master over https"])";
constexpr std::string_view ONE_NOTE_LINE = "Serve the master over https";

/** @brief The Linux package of release v1.0.0, with a digest of the right
 * shape. */
constexpr std::string_view GOOD_LINUX_PACKAGE =
    R"({"file":"R-Type-client-1.1.0-Linux.AppImage","size":37345784,)"
    R"("sha256":"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"})";
constexpr std::string_view GOOD_LINUX_FILE =
    "R-Type-client-1.1.0-Linux.AppImage";
constexpr std::size_t GOOD_LINUX_SIZE = 37345784;

/** @brief Packages whose file name leaves the download folder, or hides. */
constexpr std::string_view PACKAGE_GOING_UP =
    R"({"file":"../evil","size":1,"sha256":"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"})";
constexpr std::string_view PACKAGE_IN_A_FOLDER =
    R"({"file":"a/b","size":1,"sha256":"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"})";
constexpr std::string_view HIDDEN_PACKAGE =
    R"({"file":".hidden","size":1,"sha256":"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"})";

/** @brief Packages of size 0, one past MAXIMUM_PACKAGE_BYTES, and negative. */
constexpr std::string_view EMPTY_PACKAGE =
    R"({"file":"a","size":0,"sha256":"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"})";
constexpr std::string_view PACKAGE_TOO_LARGE =
    R"({"file":"a","size":268435457,"sha256":"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"})";
constexpr std::string_view PACKAGE_OF_NEGATIVE_SIZE =
    R"({"file":"a","size":-1,"sha256":"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"})";

/** @brief Packages whose digest has 63 digits, or uppercase ones. */
constexpr std::string_view PACKAGE_OF_SHORT_DIGEST =
    R"({"file":"a","size":1,"sha256":"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"})";
constexpr std::string_view PACKAGE_OF_UPPERCASE_DIGEST =
    R"({"file":"a","size":1,"sha256":"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"})";

/** @brief Nine notes, one past MAXIMUM_NOTE_LINES, then one note of 49
 * characters, one past MAXIMUM_NOTE_SIZE. */
constexpr std::string_view NINE_NOTES =
    R"(["1","2","3","4","5","6","7","8","9"])";
constexpr std::string_view NOTE_TOO_LONG =
    R"(["xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"])";

/** @brief Texts that are no manifest. */
constexpr std::string_view NOT_JSON = "not json";
constexpr std::string_view JSON_ARRAY = "[]";

}  // namespace rtype::client::testing

#pragma once

#include <string_view>

namespace rtype::client::testing {

/** @brief The folders of the temporary folder the tests write in. */
/** @brief One folder per test: ctest runs the tests side by side. */
constexpr std::string_view WRITTEN_NOTES_FOLDER = "r-type-written-notes-test";
constexpr std::string_view MISSING_NOTES_FOLDER = "r-type-missing-notes-test";
constexpr std::string_view BROKEN_NOTES_FOLDER = "r-type-broken-notes-test";
constexpr std::string_view INSTALL_TEST_FOLDER = "r-type-install-test";

/** @brief The notes the tests keep, and the version they describe. */
constexpr std::string_view NOTES_VERSION = "1.1.0";
constexpr std::string_view NOTE_LINE = "Leave a game from the options";

/** @brief A notes file whose line is a number, not a text. */
constexpr std::string_view NOTES_OF_A_NUMBER =
    R"({"version":"1.1.0","notes":[1]})";

/** @brief An installed AppImage and the package that replaces it, by their
 * names and what they hold. */
constexpr std::string_view APPIMAGE_NAME = "R-Type.AppImage";
constexpr std::string_view PARTIAL_APPIMAGE_NAME = "R-Type.AppImage.part";
constexpr std::string_view PACKAGE_NAME = "package";
constexpr std::string_view OLD_CONTENT = "old";
constexpr std::string_view NEW_CONTENT = "new";

}  // namespace rtype::client::testing

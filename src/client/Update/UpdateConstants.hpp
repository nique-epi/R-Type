#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::client {

/** @brief Name of the logger of the update module. */
constexpr std::string_view UPDATE_LOGGER_NAME = "Update";

/** @brief Progress a download reports, from 0 to FULL_DOWNLOAD_PERCENT. */
constexpr int FULL_DOWNLOAD_PERCENT = 100;

/** @brief File of the settings folder that keeps the notes of the release
 * being installed, until WHAT'S NEW shows them, and its members. */
constexpr std::string_view RELEASE_NOTES_FILE_NAME = "update-notes.json";
constexpr std::string_view NOTES_VERSION_KEY = "version";
constexpr std::string_view NOTES_LINES_KEY = "notes";

/** @brief The notes WHAT'S NEW holds: as many lines as its panel, each as
 * wide as it, of the characters its font draws. */
constexpr std::size_t MAXIMUM_NOTE_LINES = 8;
constexpr std::size_t MAXIMUM_NOTE_SIZE = 48;
constexpr char FIRST_PRINTABLE_CHARACTER = ' ';
constexpr char LAST_PRINTABLE_CHARACTER = '~';

/** @brief The marks a Markdown list item starts with, dropped from a note:
 * the panel draws its own bullet. */
constexpr std::string_view LIST_ITEM_MARKS = "-*+";
/** @brief What ends a line of the notes, and the blanks dropped around it. */
constexpr char NOTE_LINE_BREAK = '\n';
constexpr std::string_view NOTE_BLANKS = " \t\r";

}  // namespace rtype::client

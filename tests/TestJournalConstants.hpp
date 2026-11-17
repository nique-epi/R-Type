#pragma once

#include <string_view>

/** @brief What ends the name of the JSON journal of a DebugJournal, so it never
 * shares the file of a text one in the same test. */
constexpr std::string_view JSON_JOURNAL_SUFFIX = "_json";

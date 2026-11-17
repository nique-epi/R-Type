#pragma once

#include <string_view>
#include "JournalLineParts.hpp"

/** @returns The parts of a line of the tests whose message is @p message. */
[[nodiscard]] rtype::logging::JournalLineParts partsWithMessage(
    std::string_view message);

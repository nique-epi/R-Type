#pragma once

#include <string>
#include "JournalLineParts.hpp"
#include "LogEvent.hpp"

namespace rtype::logging {

/**
 * @returns @p parts as one JSON object, without the end of line, its members in
 * the order time, level, logger, message, then the members of parts.fields:
 * a line of JSON Lines. Every value is escaped, so no text a sender wrote can
 * end a value or add a member, and a byte that is not UTF-8 becomes U+FFFD
 * instead of failing.
 */
[[nodiscard]] std::string jsonLineOf(const JournalLineParts& parts);

/**
 * @returns The JSON object of @p event without its message: "event" and its
 * name, then each field in order, numbers and truths as JSON numbers and
 * booleans. A field named like a member every line has is left out, so the
 * line keeps one value per name.
 */
[[nodiscard]] std::string eventFieldsText(const LogEvent& event);

}  // namespace rtype::logging

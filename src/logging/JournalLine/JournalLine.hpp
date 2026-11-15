#pragma once

#include <string>
#include "JournalLineParts.hpp"

namespace rtype::logging {

/**
 * @returns @p parts as one JSON object, without the end of line, its members in
 * the order time, level, logger, message: a line of JSON Lines. Every value is
 * escaped, so no text a sender wrote can end a value or add a member, and a
 * byte that is not UTF-8 becomes U+FFFD instead of failing.
 */
[[nodiscard]] std::string jsonLineOf(const JournalLineParts& parts);

}  // namespace rtype::logging

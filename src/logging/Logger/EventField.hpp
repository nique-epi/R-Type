#pragma once

#include <cstdint>
#include <string_view>
#include <variant>

namespace rtype::logging {

/** @brief The value of a field of an event: a whole number, a text or a
 * truth. */
using EventFieldValue = std::variant<std::int64_t, std::string_view, bool>;

/**
 * @brief One named value of an event, such as the number of a player. Its name
 * is a constant of the code, never a text a sender wrote; a field named like a
 * member every line has (time, level, logger, message, event) is left out.
 */
struct EventField {
  std::string_view name;
  EventFieldValue value{std::int64_t{0}};
};

}  // namespace rtype::logging

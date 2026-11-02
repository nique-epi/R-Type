#pragma once

#include <string>
#include <string_view>

namespace rtype::engine {

/**
 * @brief Builds the line `set <name> <value>`.
 *
 * The value is wrapped in quotes when it is empty or holds a space or a
 * comment mark, so it reaches the `set` command as one word.
 *
 * @throws CommandRefusedException if the value holds a quote, which a line of
 * commands cannot carry inside a word.
 */
[[nodiscard]] std::string settingLine(std::string_view name,
                                      std::string_view value);

/**
 * @brief Builds the line `set <name> <value>` from @p assignment, written
 * `name=value` as a launch option gives it; the first `=` ends the name.
 *
 * @throws CommandRefusedException if there is no `=`, if the name is empty, or
 * if the value holds a quote.
 */
[[nodiscard]] std::string settingLineFromAssignment(
    std::string_view assignment);

}  // namespace rtype::engine

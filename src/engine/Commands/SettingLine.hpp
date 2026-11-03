#pragma once

#include <string>
#include <string_view>

namespace rtype::engine {

/**
 * @brief Builds the line `set <name> <value>`.
 *
 * The value is wrapped in quotes when it is empty or holds a space, a comment
 * mark or a quote, with each quote and backslash escaped by a backslash, so it
 * reaches the `set` command as one word, exactly as given.
 */
[[nodiscard]] std::string settingLine(std::string_view name,
                                      std::string_view value);

/**
 * @brief Builds the line `set <name> <value>` from @p assignment, written
 * `name=value` as a launch option gives it; the first `=` ends the name.
 *
 * @throws CommandRefusedException if there is no `=` or if the name is empty.
 */
[[nodiscard]] std::string settingLineFromAssignment(
    std::string_view assignment);

/**
 * @brief Builds the line of the assignment a `--set` launch option carries.
 * @throws InvalidLaunchOptionValueException naming the set option if
 * @p assignment is not `name=value`.
 */
[[nodiscard]] std::string settingLineFromSetOption(std::string_view assignment);

}  // namespace rtype::engine

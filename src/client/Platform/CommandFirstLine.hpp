#pragma once

#include <string>

namespace rtype::client {

/**
 * @brief The first line @p command prints, cut to COMMAND_ANSWER_LENGTH
 * characters, or an empty text when the command cannot run or prints
 * nothing. Defined on Linux only.
 */
[[nodiscard]] std::string firstLineOf(const char* command);

}  // namespace rtype::client

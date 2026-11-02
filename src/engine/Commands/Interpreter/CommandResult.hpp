#pragma once

#include <string>

namespace rtype::engine {

/**
 * @brief What executing one line of commands did.
 */
struct CommandResult {
  /** @brief Whether the line ran: true for a command run, a blank line or a
   * comment. */
  bool accepted{false};
  /** @brief Empty when the line was accepted; otherwise it names the command
   * and the line. */
  std::string message;
};

}  // namespace rtype::engine

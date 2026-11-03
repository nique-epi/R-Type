#pragma once

#include <string>

namespace rtype::engine {

/**
 * @brief What a command is called, the arguments it takes and what it does.
 */
struct CommandDefinition {
  /** @brief The word that starts a line running the command. */
  std::string name;
  /** @brief The shape of the arguments, as help shows it: "<name> on|off". */
  std::string arguments;
  /** @brief One sentence on what the command does. */
  std::string help;
};

}  // namespace rtype::engine

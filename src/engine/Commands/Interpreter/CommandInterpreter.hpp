#pragma once

#include <map>
#include <string>
#include <string_view>
#include <vector>
#include "CommandDefinition.hpp"
#include "CommandHandler.hpp"
#include "CommandResult.hpp"

namespace rtype::engine {

/**
 * @brief The table of the named commands of a program, and the one place that
 * runs a line of text as a command.
 *
 * A part of the program registers its commands when it installs; the file of
 * commands, the administration console and the orders of the master all hand a
 * line to execute(). A line that cannot run is answered with a message, never
 * thrown, so a bad line never stops the program.
 */
class CommandInterpreter {
 public:
  /**
   * @brief Registers @p definition, run by @p handler.
   * @throws DuplicateCommandException if a command already has that name.
   */
  void add(CommandDefinition definition, CommandHandler handler);

  /**
   * @brief Runs one line. A blank line or a comment is accepted and runs
   * nothing.
   * @returns Whether the line ran, and otherwise a message naming the command
   * and the line: an unknown command, or the reason its handler refused.
   */
  [[nodiscard]] CommandResult execute(std::string_view line) const;

  /** @returns One "name arguments - help" line per command, sorted by name. */
  [[nodiscard]] std::vector<std::string> help() const;

 private:
  std::map<std::string, CommandDefinition> definitions_;
  std::map<std::string, CommandHandler> handlers_;
};

}  // namespace rtype::engine

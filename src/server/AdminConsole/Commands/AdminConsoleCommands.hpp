#pragma once

#include <chrono>
#include <functional>
#include <span>
#include <string>
#include <vector>
#include "AdminConsoleParts.hpp"
#include "CommandDefinition.hpp"
#include "CommandInterpreter.hpp"

namespace rtype::server {

/**
 * @brief The commands of the administration console: games, players,
 * close-game, kick, network and help. They are registered in the interpreter of
 * the program, so a line runs the same typed in the console or read in
 * server.cfg; each writes its answer to the output of its parts.
 *
 * Use it on the simulation thread, or before the loop starts: the commands
 * read the registry of the games, which only that thread changes.
 */
class AdminConsoleCommands {
 public:
  explicit AdminConsoleCommands(AdminConsoleParts parts);

  /**
   * @brief Registers the six commands in @p interpreter.
   * @throws engine::DuplicateCommandException when a command of one of
   * their names was registered before.
   */
  void registerInto(engine::CommandInterpreter& interpreter);

 private:
  /** @brief Registers the commands that list: games, players, network and
   * help. */
  void addListings(engine::CommandInterpreter& interpreter);
  /** @brief Registers the commands that act on a game: close-game and kick. */
  void addActions(engine::CommandInterpreter& interpreter);
  /** @brief Registers @p definition, a command that takes no argument and
   * writes the lines @p lines computes when it runs. */
  void addListing(engine::CommandInterpreter& interpreter,
                  engine::CommandDefinition definition,
                  std::function<std::vector<std::string>()> lines);
  /** @throws engine::CommandRefusedException when @p words is not one place
   * a game holds; nothing is sent then. */
  void closeGame(std::span<const std::string> words) const;
  /** @brief Sends FAREWELL_COPIES orders to expel the nickname @p words names
   * to each game that holds a pilot of that nickname (see
   * network::sameNickname()), and writes a line for each.
   * @throws engine::CommandRefusedException when @p words is not one
   * nickname, or no game holds a pilot of that nickname; nothing is sent
   * then. */
  void kickPilot(std::span<const std::string> words) const;
  /** @returns The traffic of the socket of the supervisor, the datagrams
   * rejected since the start, then the measures of the games. */
  [[nodiscard]] std::vector<std::string> networkLines() const;
  void write(const std::vector<std::string>& lines) const;

  AdminConsoleParts parts_;
  std::chrono::steady_clock::time_point startedAt_;
};

}  // namespace rtype::server

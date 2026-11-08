#pragma once

#include <span>
#include <string>
#include "BanCommandsParts.hpp"
#include "CommandInterpreter.hpp"

namespace rtype::server {

/**
 * @brief The commands of the bans of the console: ban, unban and bans. They
 * change and read the BanBook, which writes the file of the server the games
 * read again every second (see BanWatch), and write their answer to the output
 * of their parts.
 *
 * Use it on the simulation thread only, as the BanBook.
 */
class BanCommands {
 public:
  explicit BanCommands(BanCommandsParts parts);

  /**
   * @brief Registers the three commands in @p interpreter.
   * @throws engine::DuplicateCommandException when a command of one of their
   * names was registered before.
   */
  void registerInto(engine::CommandInterpreter& interpreter);

 private:
  /** @brief Bans the target the first word names, for the length the second
   * one gives, or for good without one; a ban of that target is replaced.
   * @throws engine::CommandRefusedException when @p words is not a target and
   * at most a length, or the file refuses the ban; nothing changes then. */
  void ban(std::span<const std::string> words) const;

  /** @brief Lifts the ban of the target @p words names.
   * @throws engine::CommandRefusedException when @p words is not one target,
   * no ban names it, or the file refuses it. */
  void unban(std::span<const std::string> words) const;

  /** @brief Writes one line per ban in force, or NO_BAN_TEXT.
   * @throws engine::CommandRefusedException when @p words is not empty. */
  void listBans(std::span<const std::string> words) const;

  BanCommandsParts parts_;
};

}  // namespace rtype::server

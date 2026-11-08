#include "BanCommands.hpp"
#include <SQLiteCpp/Exception.h>
#include <chrono>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "AdminConsoleConstants.hpp"
#include "Ban.hpp"
#include "BanBook.hpp"
#include "BanCommandsParts.hpp"
#include "BanTarget.hpp"
#include "BanWords.hpp"
#include "CommandDefinition.hpp"
#include "CommandInterpreter.hpp"
#include "CommandWords.hpp"
#include "ConsoleTexts.hpp"
#include "EngineException.hpp"

namespace rtype::server {

namespace {

/** @returns "expected: <name> <arguments>", the refusal of a line with the
 * wrong words. */
engine::CommandRefusedException usageRefusal(std::string_view name,
                                             std::string_view arguments) {
  return engine::CommandRefusedException("expected: " + std::string{name} +
                                         " " + std::string{arguments});
}

/** @returns The target @p word names.
 * @throws engine::CommandRefusedException when it names none. */
BanTarget targetNamedBy(const std::string& word) {
  const std::optional<BanTarget> target = banTargetOf(word);
  if (!target.has_value()) {
    throw engine::CommandRefusedException(
        "'" + word + "' is neither a nickname nor an IPv4 address");
  }
  return *target;
}

/** @returns The end of a ban from @p now for the length @p words gives after
 * its target; nothing for a ban for good.
 * @throws engine::CommandRefusedException when that word is no length. */
std::optional<std::chrono::sys_seconds> endNamedBy(
    std::span<const std::string> words, std::chrono::sys_seconds now) {
  if (words.size() < 2) {
    return std::nullopt;
  }
  const std::optional<std::chrono::seconds> length = banLengthOf(words[1]);
  if (!length.has_value()) {
    throw engine::CommandRefusedException("'" + words[1] +
                                          "' is not a length: 30m, 2h, 7d");
  }
  return now + *length;
}

}  // namespace

BanCommands::BanCommands(BanCommandsParts parts) : parts_(std::move(parts)) {}

void BanCommands::registerInto(engine::CommandInterpreter& interpreter) {
  interpreter.add(
      engine::CommandDefinition{
          .name = std::string{BAN_COMMAND_NAME},
          .arguments = std::string{BAN_ARGUMENTS},
          .help = "Bars a nickname or an address, for good or for a length "
                  "such as 30m, 2h or 7d; a pilot it bars is expelled"},
      [this](std::span<const std::string> words) { ban(words); });
  interpreter.add(
      engine::CommandDefinition{.name = std::string{UNBAN_COMMAND_NAME},
                                .arguments = std::string{UNBAN_ARGUMENTS},
                                .help = "Lifts the ban of that target"},
      [this](std::span<const std::string> words) { unban(words); });
  interpreter.add(
      engine::CommandDefinition{.name = std::string{BANS_COMMAND_NAME},
                                .arguments = "",
                                .help = "Lists the bans in force"},
      [this](std::span<const std::string> words) { listBans(words); });
}

void BanCommands::ban(std::span<const std::string> words) const {
  if (words.empty() || words.size() > 2) {
    throw usageRefusal(BAN_COMMAND_NAME, BAN_ARGUMENTS);
  }
  const std::chrono::sys_seconds now = parts_.now();
  const Ban ban{.target = targetNamedBy(words.front()),
                .bannedAt = now,
                .expiresAt = endNamedBy(words, now)};
  try {
    parts_.book.get().put(ban);
  } catch (const SQLite::Exception& error) {
    throw engine::CommandRefusedException(
        std::string{"the file refused the ban: "} + error.what());
  }
  parts_.output.get().write(banLineOf(ban));
}

void BanCommands::unban(std::span<const std::string> words) const {
  if (words.size() != 1) {
    throw usageRefusal(UNBAN_COMMAND_NAME, UNBAN_ARGUMENTS);
  }
  const BanTarget target = targetNamedBy(words.front());
  bool removed = false;
  try {
    removed = parts_.book.get().remove(target);
  } catch (const SQLite::Exception& error) {
    throw engine::CommandRefusedException(
        std::string{"the file refused the unban: "} + error.what());
  }
  if (!removed) {
    throw engine::CommandRefusedException("no ban on " + banTargetText(target));
  }
  parts_.output.get().write(banTargetText(target) + " unbanned");
}

void BanCommands::listBans(std::span<const std::string> words) const {
  requireNoWord(words, BANS_COMMAND_NAME);
  const std::chrono::sys_seconds now = parts_.now();
  const std::vector<Ban> standing = parts_.book.get().list().inForceAt(now);
  if (standing.empty()) {
    parts_.output.get().write(std::string{NO_BAN_TEXT});
    return;
  }
  for (const Ban& ban : standing) {
    parts_.output.get().write(standingBanLineOf(ban, now));
  }
}

}  // namespace rtype::server

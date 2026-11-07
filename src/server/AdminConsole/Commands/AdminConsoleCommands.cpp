#include "AdminConsoleCommands.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "AdminConsoleConstants.hpp"
#include "AdminConsoleParts.hpp"
#include "CommandDefinition.hpp"
#include "CommandInterpreter.hpp"
#include "ConsoleTexts.hpp"
#include "EngineException.hpp"
#include "ExpulsionOrder.hpp"
#include "GameOrders.hpp"
#include "GameRegistry.hpp"
#include "IDatagramSender.hpp"
#include "ModuleConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "SupervisedGame.hpp"
#include "TrafficTotals.hpp"
#include "WholeNumber.hpp"
#include "WireTypes.hpp"

namespace rtype::server {

namespace {

/** @throws engine::CommandRefusedException when @p words is not empty. */
void requireNoWord(std::span<const std::string> words,
                   std::string_view command) {
  if (!words.empty()) {
    throw engine::CommandRefusedException("expected: " + std::string{command});
  }
}

/** @returns The game of @p games in the place @p words names.
 * @throws engine::CommandRefusedException when @p words is not one whole
 * number, or no game holds that place. */
const SupervisedGame& gameNamedBy(std::span<const std::string> words,
                                  const GameRegistry& games) {
  if (words.size() != 1) {
    throw engine::CommandRefusedException(
        "expected: " + std::string{CLOSE_GAME_COMMAND_NAME} + " " +
        std::string{CLOSE_GAME_ARGUMENTS});
  }
  const std::int64_t place = engine::wholeNumberFrom(words.front());
  const SupervisedGame* const game =
      std::in_range<std::uint16_t>(place)
          ? games.findBySlot(static_cast<std::uint16_t>(place))
          : nullptr;
  if (game == nullptr) {
    throw engine::CommandRefusedException(
        "no game holds place " + words.front() + "; " +
        std::string{GAMES_COMMAND_NAME} + " lists them");
  }
  return *game;
}

/** @brief Queues FAREWELL_COPIES copies of @p order on @p sender: nothing
 * repeats an order to a game, so each goes out that many times in a row. */
void queueCopies(IDatagramSender& sender,
                 const network::OutgoingDatagram& order) {
  for (std::size_t copy = 0; copy < FAREWELL_COPIES; ++copy) {
    sender.queueDatagram(order);
  }
}

/** @returns The nickname @p words names.
 * @throws engine::CommandRefusedException when it is not one nickname the
 * protocol accepts. */
const std::string& nicknameNamedBy(std::span<const std::string> words) {
  if (words.size() != 1) {
    throw engine::CommandRefusedException(
        "expected: " + std::string{KICK_COMMAND_NAME} + " " +
        std::string{KICK_ARGUMENTS});
  }
  if (!network::isValidNickname(words.front())) {
    throw engine::CommandRefusedException("'" + words.front() +
                                          "' is not a nickname");
  }
  return words.front();
}

/** @returns Whether @p game holds a pilot named @p nickname. */
bool holdsPilot(const SupervisedGame& game, const std::string& nickname) {
  return std::ranges::any_of(game.nicknames,
                             [&nickname](const std::string& pilot) {
                               return network::sameNickname(pilot, nickname);
                             });
}

/** @returns The games of @p games that hold a pilot named @p nickname.
 * @throws engine::CommandRefusedException when none does. */
std::vector<const SupervisedGame*> gamesPlaying(const GameRegistry& games,
                                                const std::string& nickname) {
  std::vector<const SupervisedGame*> playing;
  for (const SupervisedGame& game : games.all()) {
    if (holdsPilot(game, nickname)) {
      playing.push_back(&game);
    }
  }
  if (playing.empty()) {
    throw engine::CommandRefusedException(
        "no game holds a pilot named " + nickname + "; " +
        std::string{PLAYERS_COMMAND_NAME} + " lists them");
  }
  return playing;
}

}  // namespace

AdminConsoleCommands::AdminConsoleCommands(AdminConsoleParts parts)
    : parts_(parts), startedAt_(std::chrono::steady_clock::now()) {}

void AdminConsoleCommands::registerInto(
    engine::CommandInterpreter& interpreter) {
  addListings(interpreter);
  addActions(interpreter);
}

void AdminConsoleCommands::addListings(
    engine::CommandInterpreter& interpreter) {
  addListing(
      interpreter,
      engine::CommandDefinition{.name = std::string{GAMES_COMMAND_NAME},
                                .arguments = "",
                                .help = "Lists the games of this server"},
      [this] { return gameLinesOf(parts_.games.get().all()); });
  addListing(
      interpreter,
      engine::CommandDefinition{.name = std::string{PLAYERS_COMMAND_NAME},
                                .arguments = "",
                                .help = "Lists the pilots of each game"},
      [this] { return playerLinesOf(parts_.games.get().all()); });
  addListing(interpreter,
             engine::CommandDefinition{
                 .name = std::string{NETWORK_COMMAND_NAME},
                 .arguments = "",
                 .help = "Shows the traffic, the rejected datagrams and the "
                         "measures of the games"},
             [this] { return networkLines(); });
  addListing(interpreter,
             engine::CommandDefinition{.name = std::string{HELP_COMMAND_NAME},
                                       .arguments = "",
                                       .help = "Lists the commands"},
             [this] { return parts_.interpreter.get().help(); });
}

void AdminConsoleCommands::addActions(engine::CommandInterpreter& interpreter) {
  interpreter.add(
      engine::CommandDefinition{
          .name = std::string{CLOSE_GAME_COMMAND_NAME},
          .arguments = std::string{CLOSE_GAME_ARGUMENTS},
          .help = "Stops the game in that place; its pilots are told"},
      [this](std::span<const std::string> words) { closeGame(words); });
  interpreter.add(
      engine::CommandDefinition{
          .name = std::string{KICK_COMMAND_NAME},
          .arguments = std::string{KICK_ARGUMENTS},
          .help = "Expels the pilot of that nickname from its game"},
      [this](std::span<const std::string> words) { kickPilot(words); });
}

void AdminConsoleCommands::addListing(
    engine::CommandInterpreter& interpreter,
    engine::CommandDefinition definition,
    std::function<std::vector<std::string>()> lines) {
  std::string name = definition.name;
  interpreter.add(std::move(definition),
                  [this, name = std::move(name), lines = std::move(lines)](
                      std::span<const std::string> words) {
                    requireNoWord(words, name);
                    write(lines());
                  });
}

void AdminConsoleCommands::closeGame(std::span<const std::string> words) const {
  const SupervisedGame& game = gameNamedBy(words, parts_.games.get());
  queueCopies(parts_.sender.get(), stopOrderTo(game));
  parts_.output.get().write(closingLineOf(game));
}

void AdminConsoleCommands::kickPilot(std::span<const std::string> words) const {
  const std::string& nickname = nicknameNamedBy(words);
  for (const SupervisedGame* const game :
       gamesPlaying(parts_.games.get(), nickname)) {
    queueCopies(parts_.sender.get(),
                orderTo(*game, network::ExpulsionOrder{.nickname = nickname}));
    parts_.output.get().write(kickLineOf(*game, nickname));
  }
}

std::vector<std::string> AdminConsoleCommands::networkLines() const {
  std::vector<std::string> lines =
      networkLinesOf(network::trafficTotalsOf(parts_.traffic.get().report()),
                     std::chrono::steady_clock::now() - startedAt_,
                     parts_.rejections.get().rejectedSinceStart());
  lines.push_back(measuresLineOf(parts_.measures.get().value()));
  return lines;
}

void AdminConsoleCommands::write(const std::vector<std::string>& lines) const {
  for (const std::string& line : lines) {
    parts_.output.get().write(line);
  }
}

}  // namespace rtype::server

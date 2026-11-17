#include "GameSpawner.hpp"
#include <array>
#include <cstdint>
#include <exception>
#include <limits>
#include <memory>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include "Endpoint.hpp"
#include "EventField.hpp"
#include "GameCommand.hpp"
#include "GameLaunch.hpp"
#include "GameOpening.hpp"
#include "GameRules.hpp"
#include "GameSpawnerParts.hpp"
#include "IChildProcess.hpp"
#include "LogEvent.hpp"
#include "LogLevel.hpp"
#include "Logger.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"
#include "SupervisedGamesConstants.hpp"

namespace rtype::server {

namespace {

/** @brief Writes the info event of the game @p launch started: its number and
 * the port it listens on. */
void writeStarted(const logging::Logger& logger, const GameLaunch& launch) {
  std::ostringstream message;
  message << "game " << launch.number << " started on port " << launch.port;
  const std::string text = message.str();
  const std::array fields{
      logging::EventField{.name = GAME_FIELD,
                          .value = std::int64_t{launch.number}},
      logging::EventField{.name = PORT_FIELD,
                          .value = std::int64_t{launch.port}}};
  logger.event(logging::LogEvent{.level = logging::LogLevel::Info,
                                 .name = GAME_STARTED_EVENT,
                                 .message = text,
                                 .fields = fields});
}

}  // namespace

GameSpawner::GameSpawner(GameSpawnerParts parts)
    : parts_(std::move(parts)), logger_(std::string{SUPERVISOR_LOGGER_NAME}) {}

bool GameSpawner::openGame(std::optional<network::Endpoint> creator,
                           network::GameRules rules) {
  const std::uint32_t number = nextNumber_;
  try {
    const GameLaunch launch{
        .number = number,
        .port = parts_.freePort(),
        .supervisorPort = parts_.supervisorPort.get().listeningPort(),
        .linkToken = drawLinkToken(),
        .rules = std::move(rules)};
    std::unique_ptr<IChildProcess> process =
        parts_.launcher.get().launch(gameCommand(parts_.settings.get(), launch,
                                                 parts_.database.get().text()));
    parts_.registry.get().open(GameOpening{.number = launch.number,
                                           .port = launch.port,
                                           .linkToken = launch.linkToken,
                                           .creator = std::move(creator),
                                           .rules = launch.rules,
                                           .process = std::move(process)});
    parts_.admission.get().admit(
        network::Endpoint{
            .address = std::string{network::LOOPBACK_IPV4_ADDRESS},
            .port = launch.port},
        launch.linkToken);
    writeStarted(logger_, launch);
  } catch (const std::exception& error) {
    logger_.error("cannot start game ", number, ": ", error.what());
    return false;
  }
  ++nextNumber_;
  return true;
}

std::uint64_t GameSpawner::drawLinkToken() {
  std::uniform_int_distribution<std::uint64_t> tokens{
      1, std::numeric_limits<std::uint64_t>::max()};
  return tokens(random_);
}

}  // namespace rtype::server

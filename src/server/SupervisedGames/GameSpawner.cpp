#include "GameSpawner.hpp"
#include <cstdint>
#include <exception>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include "Endpoint.hpp"
#include "GameCommand.hpp"
#include "GameLaunch.hpp"
#include "GameOpening.hpp"
#include "GameSpawnerParts.hpp"
#include "IChildProcess.hpp"
#include "NetworkConstants.hpp"
#include "SupervisedGamesConstants.hpp"

namespace rtype::server {

GameSpawner::GameSpawner(GameSpawnerParts parts)
    : parts_(std::move(parts)), logger_(std::string{SUPERVISOR_LOGGER_NAME}) {}

bool GameSpawner::openGame() {
  const std::uint32_t number = nextNumber_;
  try {
    const GameLaunch launch{
        .number = number,
        .port = parts_.freePort(),
        .supervisorPort = parts_.supervisorPort.get().listeningPort(),
        .linkToken = drawLinkToken()};
    std::unique_ptr<IChildProcess> process =
        parts_.launcher.get().launch(gameCommand(parts_.settings.get(), launch,
                                                 parts_.database.get().text()));
    parts_.registry.get().open(GameOpening{.number = launch.number,
                                           .port = launch.port,
                                           .linkToken = launch.linkToken,
                                           .process = std::move(process)});
    parts_.admission.get().admit(
        network::Endpoint{
            .address = std::string{network::LOOPBACK_IPV4_ADDRESS},
            .port = launch.port},
        launch.linkToken);
    logger_.info("game ", launch.number, " started on port ", launch.port);
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

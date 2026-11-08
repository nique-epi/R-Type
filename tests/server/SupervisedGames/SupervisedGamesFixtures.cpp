#include "SupervisedGamesFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ConnectionRequest.hpp"
#include "Endpoint.hpp"
#include "GameLaunch.hpp"
#include "GameLaunchOptions.hpp"
#include "GameLaunchSettings.hpp"
#include "GameLifecycle.hpp"
#include "GameOpening.hpp"
#include "GameRegistry.hpp"
#include "GameRules.hpp"
#include "GameStatus.hpp"
#include "LaunchOptions.hpp"
#include "NetworkConstants.hpp"
#include "ProcessCommand.hpp"
#include "RecordingChildProcess.hpp"
#include "RecordingChildState.hpp"
#include "RecordingProcessLauncherConstants.hpp"
#include "SeatRequest.hpp"
#include "SupervisedGame.hpp"
#include "SupervisedGamesTestConstants.hpp"

std::uint16_t portOfGame(std::uint32_t number) {
  return static_cast<std::uint16_t>(FIRST_GAME_PORT + number);
}

std::uint64_t tokenOfGame(std::uint32_t number) {
  return FIRST_LINK_TOKEN + number;
}

rtype::server::GameOpening openingOf(
    std::uint32_t number, const std::shared_ptr<RecordingChildState>& state,
    std::optional<rtype::network::Endpoint> creator) {
  return rtype::server::GameOpening{
      .number = number,
      .port = portOfGame(number),
      .linkToken = tokenOfGame(number),
      .creator = std::move(creator),
      .process = std::make_unique<RecordingChildProcess>(
          FIRST_PROCESS_ID + number, state)};
}

void openReporting(rtype::server::GameRegistry& registry, std::uint32_t number,
                   const rtype::network::GameStatus& status) {
  registry.open(openingOf(number, std::make_shared<RecordingChildState>()));
  registry.record(*registry.findByToken(tokenOfGame(number)), SEQUENCE_FIVE,
                  status);
}

rtype::network::GameChoice choiceOfGame(std::uint16_t slot) {
  return rtype::network::GameChoice{
      .kind = rtype::network::GameChoiceKind::ExistingGame, .number = slot};
}

rtype::network::GameStatus statusWithPlayers(
    rtype::network::GameStatusState state, std::size_t playerCount) {
  rtype::network::GameStatus status{
      .state = state, .nicknames = {}, .measures = {}};
  for (std::size_t player = 0;
       player < playerCount && player < SUPERVISED_NICKNAMES.size(); ++player) {
    status.nicknames.emplace_back(SUPERVISED_NICKNAMES.at(player));
  }
  return status;
}

rtype::server::SupervisedGame supervisedGameIn(
    std::uint16_t slot, rtype::server::GameLifecycle lifecycle,
    std::size_t playerCount) {
  rtype::server::SupervisedGame game{
      .number = slot,
      .slot = slot,
      .lifecycle = lifecycle,
      .nicknames = statusWithPlayers(
                       rtype::network::GameStatusState::InProgress, playerCount)
                       .nicknames,
      .measures = {},
      .port = portOfGame(slot),
      .linkToken = tokenOfGame(slot),
      .newestSequence = std::nullopt,
      .reservations = {},
      .creator = std::nullopt,
      .process = nullptr};
  return game;
}

std::string argumentAfter(const rtype::server::ProcessCommand& command,
                          std::string_view option) {
  for (std::size_t index = 0; index + 1 < command.arguments.size(); ++index) {
    if (command.arguments[index] == option) {
      return command.arguments[index + 1];
    }
  }
  return {};
}

rtype::network::Endpoint supervisedPlayer(std::size_t index) {
  return rtype::network::Endpoint{
      .address = std::string{SUPERVISED_PLAYER_ADDRESS},
      .port = static_cast<std::uint16_t>(FIRST_SUPERVISED_PLAYER_PORT + index)};
}

rtype::server::SeatRequest seatRequestOf(std::size_t index) {
  return rtype::server::SeatRequest{
      .sender = supervisedPlayer(index),
      .nickname =
          std::string{SUPERVISED_NICKNAME_PREFIX} + std::to_string(index + 1)};
}

rtype::network::Endpoint loopbackOn(std::uint16_t port) {
  return rtype::network::Endpoint{
      .address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
      .port = port};
}

rtype::server::GameLaunchSettings settingsOf(
    const rtype::server::LaunchOptions& supervisor) {
  return rtype::server::GameLaunchSettings{
      .program = PROGRAM_FOR_TESTS,
      .folder = FOLDER_FOR_TESTS,
      .gameLines = {},
      .forwardedArguments = supervisor.gameArguments(),
      .wavesFile = {},
      .contentFolders = {},
      .trafficCsvFile = supervisor.trafficCsvFile()};
}

rtype::network::GameRules chosenRules() {
  return rtype::network::GameRules{.difficulty = std::string{CHOSEN_DIFFICULTY},
                                   .lives = CHOSEN_LIVES,
                                   .friendlyFire = true};
}

rtype::server::GameLaunch launchOfGameThree() {
  return rtype::server::GameLaunch{
      .number = GAME_THREE,
      .port = FIRST_GAME_PORT,
      .supervisorPort = FIRST_SUPERVISED_PLAYER_PORT,
      .linkToken = FIRST_LINK_TOKEN};
}

rtype::server::GameLaunchOptions gameOptionsOf(
    const rtype::server::ProcessCommand& command) {
  const std::vector<std::string_view> arguments{
      command.arguments.begin() + GAME_MODE_ARGUMENT_COUNT,
      command.arguments.end()};
  return rtype::server::GameLaunchOptions::read(arguments);
}

#include "SupervisedGamesFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include "Endpoint.hpp"
#include "GameOpening.hpp"
#include "NetworkConstants.hpp"
#include "ProcessCommand.hpp"
#include "RecordingChildProcess.hpp"
#include "RecordingChildState.hpp"
#include "RecordingProcessLauncherConstants.hpp"
#include "SupervisedGamesTestConstants.hpp"

std::uint16_t portOfGame(std::uint32_t number) {
  return static_cast<std::uint16_t>(FIRST_GAME_PORT + number);
}

std::uint64_t tokenOfGame(std::uint32_t number) {
  return FIRST_LINK_TOKEN + number;
}

rtype::server::GameOpening openingOf(
    std::uint32_t number, const std::shared_ptr<RecordingChildState>& state) {
  return rtype::server::GameOpening{
      .number = number,
      .port = portOfGame(number),
      .linkToken = tokenOfGame(number),
      .process = std::make_unique<RecordingChildProcess>(
          FIRST_PROCESS_ID + number, state)};
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

rtype::network::Endpoint loopbackOn(std::uint16_t port) {
  return rtype::network::Endpoint{
      .address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
      .port = port};
}

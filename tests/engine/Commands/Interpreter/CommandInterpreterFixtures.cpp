#include "CommandInterpreterFixtures.hpp"
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "CommandHandler.hpp"
#include "CommandInterpreterTestConstants.hpp"
#include "EngineException.hpp"

rtype::engine::CommandHandler recordingHandler(
    std::vector<std::string>& received) {
  return [&received](std::span<const std::string> words) {
    received.assign(words.begin(), words.end());
  };
}

rtype::engine::CommandHandler refusingHandler(std::string_view reason) {
  return [reason](std::span<const std::string>) {
    throw rtype::engine::CommandRefusedException(std::string(reason));
  };
}

std::string lineRunning(std::string_view command) {
  return std::string(command) + " " + std::string(FIRST_ARGUMENT) + " " +
         std::string(SECOND_ARGUMENT);
}

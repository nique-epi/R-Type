#include "ServerException.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include "DataFilePlace.hpp"
#include "ErrorText.hpp"
#include "ServerConstants.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::server {

ServerException::ServerException(const std::string& message)
    : std::runtime_error(message) {}

InvalidEnemyListException::InvalidEnemyListException(
    const engine::DataFilePlace& place, engine::WholeNumberRange length)
    : ServerException(engine::placeText(place) + ": " + place.key +
                      " must be a list of " + engine::rangeText(length) +
                      " enemy names") {}

UnknownEnemyException::UnknownEnemyException(
    const engine::DataFilePlace& place, const std::string& enemy,
    const std::vector<std::string>& known)
    : ServerException(engine::placeText(place) + ": " + place.key +
                      ": unknown enemy \"" + enemy +
                      "\"; known: " + engine::listText(known)) {}

UnknownDifficultyException::UnknownDifficultyException(
    const std::string& difficulty, const std::vector<std::string>& known)
    : ServerException(
          "no difficulty " + difficulty +
          " in the waves file, which describes: " + engine::listText(known)) {}

EmptyEnemyCycleException::EmptyEnemyCycleException()
    : ServerException("A wave plan must name at least one enemy") {}

NoJoinableGameException::NoJoinableGameException()
    : ServerException("no game to join: the server has not created one") {}

TooManyGamesException::TooManyGamesException(std::size_t requested,
                                             std::size_t maximum)
    : ServerException("the server cannot host " + std::to_string(requested) +
                      " games: a heartbeat lists " + std::to_string(maximum) +
                      " at most") {}

ServerAlreadyOpenException::ServerAlreadyOpenException()
    : ServerException("the server is already open") {}

ServerNotOpenException::ServerNotOpenException()
    : ServerException("the server is not open: call open() first") {}

SignalMaskException::SignalMaskException(const std::string& reason)
    : ServerException("cannot block the signals that stop the server: " +
                      reason) {}

ConsoleHandlerException::ConsoleHandlerException(std::uint32_t errorCode)
    : ServerException(
          "cannot register the handler of the console events that stop the "
          "server: Windows error " +
          std::to_string(errorCode)) {}

InvalidStateRateException::InvalidStateRateException(
    std::size_t statesPerSecond)
    : ServerException("the world state rate must be from " +
                      std::to_string(LOWEST_WORLD_STATES_PER_SECOND) + " to " +
                      std::to_string(HIGHEST_WORLD_STATES_PER_SECOND) +
                      " states per second, not " +
                      std::to_string(statesPerSecond)) {}

InvalidInputDelayException::InvalidInputDelayException(std::size_t delayTicks)
    : ServerException("the input delay must be from " +
                      std::to_string(LOWEST_INPUT_DELAY_TICKS) + " to " +
                      std::to_string(HIGHEST_INPUT_DELAY_TICKS) +
                      " ticks, not " + std::to_string(delayTicks)) {}

ProcessLaunchException::ProcessLaunchException(const std::string& program,
                                               const std::string& description)
    : ServerException("cannot start " + program + ": " + description) {}

ChildBindingException::ChildBindingException(const std::string& step)
    : ServerException("cannot tie a game process to the supervisor: " + step +
                      " failed") {}

IncompleteSupervisorLinkException::IncompleteSupervisorLinkException(
    const std::string& missingOption)
    : ServerException(
          "a game process needs both --supervisor and "
          "--link-token: " +
          missingOption + " is missing") {}

}  // namespace rtype::server

#include "ServerException.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include "ServerConstants.hpp"
#include "WaveFilePlace.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::server {

namespace {

/** @returns Where @p place is, as compilers write it: `file:line:column`. */
std::string placeText(const WaveFilePlace& place) {
  return place.file + ':' + std::to_string(place.line) + ':' +
         std::to_string(place.column);
}

/** @returns @p names separated by commas, or `none` when there is no name. */
std::string listText(const std::vector<std::string>& names) {
  if (names.empty()) {
    return "none";
  }
  std::string text = names.front();
  for (auto name = names.begin() + 1; name != names.end(); ++name) {
    text += ", " + *name;
  }
  return text;
}

/** @returns The text of the whole numbers of @p range: `1 to 100`. */
std::string rangeText(WholeNumberRange range) {
  return std::to_string(range.lowest) + " to " + std::to_string(range.highest);
}

}  // namespace

ServerException::ServerException(const std::string& message)
    : std::runtime_error(message) {}

UnreadableWaveFileException::UnreadableWaveFileException(
    const std::string& file)
    : ServerException("cannot read the waves file " + file) {}

WaveFileSyntaxException::WaveFileSyntaxException(const WaveFilePlace& place,
                                                 const std::string& description)
    : ServerException(placeText(place) + ": " + description) {}

MissingWaveFileKeyException::MissingWaveFileKeyException(
    const WaveFilePlace& place)
    : ServerException(placeText(place) + ": " + place.key + " is missing") {}

UnknownWaveFileKeyException::UnknownWaveFileKeyException(
    const WaveFilePlace& place)
    : ServerException(placeText(place) + ": unknown key " + place.key) {}

WaveFileTableExpectedException::WaveFileTableExpectedException(
    const WaveFilePlace& place)
    : ServerException(placeText(place) + ": " + place.key +
                      " must be a table") {}

WaveFileValueOutOfRangeException::WaveFileValueOutOfRangeException(
    const WaveFilePlace& place, WholeNumberRange range)
    : ServerException(placeText(place) + ": " + place.key +
                      " must be an integer from " + rangeText(range)) {}

InvalidEnemyListException::InvalidEnemyListException(const WaveFilePlace& place,
                                                     WholeNumberRange length)
    : ServerException(placeText(place) + ": " + place.key +
                      " must be a list of " + rangeText(length) +
                      " enemy names") {}

UnknownEnemyException::UnknownEnemyException(
    const WaveFilePlace& place, const std::string& enemy,
    const std::vector<std::string>& known)
    : ServerException(placeText(place) + ": " + place.key +
                      ": unknown enemy \"" + enemy +
                      "\"; known: " + listText(known)) {}

UnknownDifficultyException::UnknownDifficultyException(
    const std::string& difficulty, const std::vector<std::string>& known)
    : ServerException(
          "no difficulty " + difficulty +
          " in the waves file, which describes: " + listText(known)) {}

EmptyEnemyCycleException::EmptyEnemyCycleException()
    : ServerException("A wave plan must name at least one enemy") {}

NoJoinableGameException::NoJoinableGameException()
    : ServerException("no game to join: the server has not created one") {}

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

}  // namespace rtype::server

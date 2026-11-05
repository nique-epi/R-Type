#include "StatisticsSheet.hpp"
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include "EnemyKilled.hpp"
#include "GameStatisticsConstants.hpp"

namespace rtype::server {

namespace {

/** @returns The first two columns of a row, each followed by its comma:
 * @p event, then the number of the game @p number. */
std::string gameColumnsOf(std::string_view event, std::uint32_t number) {
  return std::string{event} + ',' + std::to_string(number) + ',';
}

}  // namespace

std::uint32_t StatisticsSheet::openGame() {
  ++lastGame_;
  pointsOfOpenGames_[lastGame_] = 0;
  return lastGame_;
}

void StatisticsSheet::addKill(std::uint32_t number,
                              const game::EnemyKilled& kill) {
  pointsOfOpenGames_[number] += kill.points;
  rows_ += gameColumnsOf(KILL_EVENT, number) +
           std::to_string(kill.playerNumber) + ',' +
           std::to_string(kill.points) + '\n';
}

void StatisticsSheet::closeGame(std::uint32_t number) {
  rows_ += gameColumnsOf(TOTAL_EVENT, number) + ',' +
           std::to_string(pointsOfOpenGames_[number]) + '\n';
  pointsOfOpenGames_.erase(number);
}

std::string StatisticsSheet::takeRows() { return std::exchange(rows_, {}); }

}  // namespace rtype::server

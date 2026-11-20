#include "MapTally.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <random>
#include <span>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "MapChoice.hpp"

namespace rtype::server {

namespace {

/** @returns How many players who are not away vote for each of @p mapCount
 * maps. */
std::vector<std::size_t> votesOf(const ConnectedPlayers& players,
                                 std::size_t mapCount) {
  std::vector<std::size_t> votes(mapCount, 0);
  for (const ConnectedPlayer* const player : players) {
    if (!player->away && player->mapVote >= 1 && player->mapVote <= mapCount) {
      ++votes.at(player->mapVote - 1);
    }
  }
  return votes;
}

}  // namespace

std::size_t votedMapOf(const ConnectedPlayers& players,
                       std::span<const MapChoice> maps, std::uint32_t seed) {
  const std::vector<std::size_t> votes = votesOf(players, maps.size());
  const std::size_t most = *std::ranges::max_element(votes);
  std::vector<std::size_t> leaders;
  for (std::size_t map = 0; map < votes.size(); ++map) {
    if (votes.at(map) == most) {
      leaders.push_back(map);
    }
  }
  std::mt19937 draws{seed};
  std::uniform_int_distribution<std::size_t> pick{0, leaders.size() - 1};
  return leaders.at(pick(draws));
}

}  // namespace rtype::server

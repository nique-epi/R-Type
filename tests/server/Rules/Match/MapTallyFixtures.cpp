#include "MapTallyFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <set>
#include <span>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "MapChoice.hpp"
#include "MapTally.hpp"
#include "MapTallyTestConstants.hpp"

namespace rtype::server::testing {

namespace {

ConnectedPlayers listOf(std::vector<ConnectedPlayer>& players) {
  ConnectedPlayers listed;
  for (ConnectedPlayer& player : players) {
    listed.append(&player);
  }
  return listed;
}

}  // namespace

std::vector<ConnectedPlayer> votingPlayers(
    std::span<const std::uint8_t> votes) {
  std::vector<ConnectedPlayer> players(votes.size());
  for (std::size_t index = 0; index < votes.size(); ++index) {
    players.at(index).playerNumber = static_cast<std::uint8_t>(index + 1);
    players.at(index).mapVote = votes[index];
  }
  return players;
}

std::set<std::size_t> mapsChosenOverSeeds(std::vector<ConnectedPlayer>& players,
                                          std::size_t mapCount) {
  const ConnectedPlayers listed = listOf(players);
  const std::vector<MapChoice> maps(mapCount);
  std::set<std::size_t> chosen;
  for (std::uint32_t seed = 0; seed < SEEDS_TRIED; ++seed) {
    chosen.insert(votedMapOf(listed, maps, seed));
  }
  return chosen;
}

std::size_t mapChosen(std::vector<ConnectedPlayer>& players,
                      std::size_t mapCount) {
  const std::vector<MapChoice> maps(mapCount);
  return votedMapOf(listOf(players), maps, 0);
}

}  // namespace rtype::server::testing

#include "ScoreBoardFixtures.hpp"
#include <cstdint>
#include <vector>
#include "LobbyState.hpp"
#include "ScoreBoard.hpp"
#include "ScoreBoardTestConstants.hpp"
#include "WorldState.hpp"

rtype::network::LobbyPlayer waiting(std::uint8_t number, const char* nickname) {
  return rtype::network::LobbyPlayer{
      .playerNumber = number, .ready = false, .nickname = nickname};
}

rtype::network::PlayerState listedPlayer(std::uint8_t number,
                                         std::uint8_t lives) {
  return rtype::network::PlayerState{.playerNumber = number,
                                     .shipEntityId = 0,
                                     .lives = lives,
                                     .score = SCORE,
                                     .invulnerable = false};
}

rtype::client::ScoreBoard scoreBoardOfThree() {
  const std::vector<rtype::network::LobbyPlayer> room{
      waiting(PLAYER_THREE, "KAI"), waiting(PLAYER_ONE, "ACE"),
      waiting(PLAYER_TWO, "NOVA")};
  return {room, LOCAL_NUMBER};
}

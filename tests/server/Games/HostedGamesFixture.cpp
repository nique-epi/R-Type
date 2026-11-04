#include "HostedGamesFixture.hpp"
#include <cstddef>
#include <string>
#include "ConnectedPlayer.hpp"
#include "GameInstance.hpp"
#include "SentDatagrams.hpp"
#include "ServerModuleTestConstants.hpp"

rtype::server::ConnectedPlayer& HostedGamesFixture::seat(
    rtype::server::GameInstance& game, std::size_t index,
    const std::string& nickname) {
  rtype::server::ConnectedPlayer* const player =
      game.roster().add(rtype::server::testing::playerEndpoint(index), nickname,
                        PLAYER_TOKEN_BASE + index);
  game.onPlayerJoined(*player);
  return *player;
}

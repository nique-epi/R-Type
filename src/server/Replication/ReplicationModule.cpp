#include "ReplicationModule.hpp"
#include <memory>
#include <string>
#include "EncodedMessage.hpp"
#include "GameContext.hpp"
#include "GameEventBroadcaster.hpp"
#include "GameEventBroadcasterParts.hpp"
#include "GameInstanceConstants.hpp"
#include "GameReplication.hpp"
#include "IGameReplication.hpp"
#include "InstallContext.hpp"
#include "MessageDelivery.hpp"

namespace rtype::server {

std::string ReplicationModule::name() const {
  return std::string{REPLICATION_MODULE_NAME};
}

void ReplicationModule::install(GameContext& game,
                                engine::InstallContext& install) {
  game_ = &game;
  replication_ = std::make_unique<GameReplication>(game);
  install.provide<IGameReplication>(*replication_);
}

void ReplicationModule::start() {
  broadcaster_ =
      std::make_unique<GameEventBroadcaster>(GameEventBroadcasterParts{
          .events = game_->events,
          .sendToEveryPlayer =
              [game = game_](const network::EncodedMessage& message) {
                sendToEveryPlayer(game->roster.get(), game->send, message);
              },
          .guaranteeToEveryPlayer =
              [game = game_](const network::EncodedMessage& message) {
                guaranteeToEveryPlayer(game->roster.get(), game->send, message);
              }});
}

void ReplicationModule::stop() { broadcaster_.reset(); }

}  // namespace rtype::server

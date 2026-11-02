#include "ReplicationModule.hpp"
#include <functional>
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
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

std::string ReplicationModule::name() const {
  return std::string{REPLICATION_MODULE_NAME};
}

void ReplicationModule::install(GameContext& game,
                                engine::InstallContext& install) {
  game_ = &game;
  replication_ = std::make_unique<GameReplication>(
      game.roster.get(), game.send, game.settings.statesPerSecond);
  install.provide<IGameReplication>(*replication_);
}

void ReplicationModule::start() {
  PlayerRoster& roster = game_->roster.get();
  const std::function<void(network::OutgoingDatagram)> send = game_->send;
  broadcaster_ =
      std::make_unique<GameEventBroadcaster>(GameEventBroadcasterParts{
          .events = game_->events,
          .sendToEveryPlayer =
              [&roster, send](const network::EncodedMessage& message) {
                sendToEveryPlayer(roster, send, message);
              },
          .guaranteeToEveryPlayer =
              [&roster, send](const network::EncodedMessage& message) {
                guaranteeToEveryPlayer(roster, send, message);
              }});
}

void ReplicationModule::stop() { broadcaster_.reset(); }

}  // namespace rtype::server

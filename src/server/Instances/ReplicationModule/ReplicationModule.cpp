#include "ReplicationModule.hpp"
#include <memory>
#include <string>
#include "Endpoint.hpp"
#include "GameContext.hpp"
#include "GameEventBroadcaster.hpp"
#include "GameInstanceConstants.hpp"
#include "GameReplication.hpp"
#include "IGameReplication.hpp"
#include "InstallContext.hpp"
#include "PlayerRoster.hpp"
#include "SessionDatagram.hpp"

namespace rtype::server {

std::string ReplicationModule::name() const {
  return std::string{REPLICATION_MODULE_NAME};
}

void ReplicationModule::install(GameContext& game,
                                engine::InstallContext& install) {
  game_ = &game;
  replication_ =
      std::make_unique<GameReplication>(game.roster.get(), game.send);
  install.provide<IGameReplication>(*replication_);
}

void ReplicationModule::start() {
  PlayerRoster& roster = game_->roster.get();
  broadcaster_ =
      std::make_unique<GameEventBroadcaster>(GameEventBroadcaster::Wiring{
          .events = game_->events,
          .timers = game_->timers,
          .recipients = [&roster] { return roster.endpoints(); },
          .headerFor =
              [&roster](const network::Endpoint& recipient) {
                return sessionHeaderFor(roster, recipient);
              },
          .send = game_->send});
}

void ReplicationModule::stop() {
  broadcaster_.reset();
  replication_.reset();
}

}  // namespace rtype::server

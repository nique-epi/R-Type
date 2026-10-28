#pragma once

#include <memory>
#include <string>
#include "GameContext.hpp"
#include "GameEventBroadcaster.hpp"
#include "GameReplication.hpp"
#include "IGameModule.hpp"
#include "InstallContext.hpp"

namespace rtype::server {

/**
 * @brief The module of a game that tells its players about it: it provides
 * IGameReplication to the rules, and broadcasts the events of the game on its
 * bus to the players of its roster.
 *
 * It is installed before the module whose rules require IGameReplication.
 * install() builds the replication and provides it; start() subscribes the
 * broadcaster to the bus; stop() destroys the broadcaster, which gives its
 * subscriptions back to the bus, then the replication.
 */
class ReplicationModule final : public IGameModule {
 public:
  [[nodiscard]] std::string name() const override;

  void install(GameContext& game, engine::InstallContext& install) override;

  void start() override;

  void stop() override;

 private:
  GameContext* game_{nullptr};
  std::unique_ptr<GameReplication> replication_;
  std::unique_ptr<GameEventBroadcaster> broadcaster_;
};

}  // namespace rtype::server

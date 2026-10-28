#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EventBus.hpp"
#include "GameContext.hpp"
#include "GameInstanceConstants.hpp"
#include "IGameModule.hpp"
#include "LoopStages.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ServiceRegistry.hpp"
#include "SessionHooks.hpp"
#include "SystemScheduler.hpp"
#include "TimerScheduler.hpp"
#include "World.hpp"
#include "WorldSlot.hpp"

namespace rtype::server::testing {

/**
 * @brief The parts of a game a module is installed in, without the game: a
 * bus, timers, a scheduler, a world, a roster, the services and the stages of
 * a tick, and the datagrams the module sends, in order.
 */
class GameModuleFixture : public ::testing::Test {
 public:
  GameModuleFixture();

 protected:
  /** @brief Installs @p module as a game installs it. */
  void install(IGameModule& module);

  /** @brief Connects the player at @p index, who stays in the roster. */
  ConnectedPlayer& join(std::size_t index);

  /** @returns The service of type @p Service a module provided, or nullptr. */
  template <typename Service>
  [[nodiscard]] Service* provided() {
    return services_.find<Service>();
  }

  template <typename Event>
  void publishAndDispatch(Event event) {
    events_.publish(std::move(event));
    events_.dispatch();
  }

  /** @brief Calls the functions the modules added to the hooks for a player
   * who joined, as GameInstance does. */
  void playerJoined(ConnectedPlayer& player);

  /** @brief Calls the functions the modules added to the hooks for a player
   * whose session closes, as GameInstance does. */
  void sessionClosing(const ConnectedPlayer& player);

  /** @returns The context the modules are installed with. */
  [[nodiscard]] GameContext& context();

  /** @brief Runs the stages of a tick, as a game does. */
  void runStages();

  [[nodiscard]] game::World& world();

  [[nodiscard]] const std::vector<network::OutgoingDatagram>& sentDatagrams()
      const;

 private:
  engine::EventBus events_;
  engine::TimerScheduler timers_;
  engine::SystemScheduler systems_;
  WorldSlot world_;
  PlayerRoster roster_;
  SessionHooks hooks_;
  engine::ServiceRegistry services_;
  engine::LoopStages stages_{GAME_STAGES};
  std::vector<network::OutgoingDatagram> sent_;
  GameContext context_;
};

}  // namespace rtype::server::testing

#pragma once

#include <memory>
#include <string>
#include "IGameModule.hpp"
#include "IRouletteDraw.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "RouletteRound.hpp"
#include "RouletteRoundAnnouncer.hpp"
#include "RouletteTable.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::server {

struct GameContext;

/**
 * @brief The module of a game that keeps the roulette table of its waiting
 * room: it seats the players who choose it, takes their bets, turns the rounds
 * and tells the room their state.
 *
 * Optional: nothing includes it but the list of the modules of a game, and a
 * game without it drops the CasinoPlace and RouletteBets of its players. The
 * table is open while the game waits for players; when the game leaves its
 * waiting room every player goes back to the room and the rounds start over.
 * start() adds its functions to the hooks of the game, which stay there after
 * stop(): the game is destroyed right after its modules stop.
 */
class CasinoModule final : public IGameModule {
 public:
  /** @brief A casino whose numbers come from the system (SystemRouletteDraw).
   */
  CasinoModule();

  /** @brief A casino whose numbers come from @p draw, which must not be null.
   */
  explicit CasinoModule(std::unique_ptr<network::IRouletteDraw> draw);

  [[nodiscard]] std::string name() const override;
  void install(GameContext& game, engine::InstallContext& install) override;
  void start() override;
  void stop() override;

 private:
  /** @brief Turns the round, writes the number it drew, and tells the room. */
  void onTick();

  GameContext* game_{nullptr};
  std::unique_ptr<network::IRouletteDraw> draw_;
  std::unique_ptr<RouletteTable> table_;
  std::unique_ptr<RouletteRoundAnnouncer> announcer_;
  engine::SubscriptionHandle stateSubscription_{};
  bool open_{true};
  network::RoulettePhase shownPhase_{network::RoulettePhase::Betting};
  logging::Logger logger_;
};

}  // namespace rtype::server

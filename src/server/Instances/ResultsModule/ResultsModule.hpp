#pragma once

#include <memory>
#include <string>
#include "GameResultKeeper.hpp"
#include "IGameFlow.hpp"
#include "IGameModule.hpp"
#include "IResultsArchive.hpp"
#include "InstallContext.hpp"
#include "LeaderboardDesk.hpp"

namespace rtype::server {

struct GameContext;

/**
 * @brief The module of a game that records its results and answers the
 * leaderboard requests of its players.
 *
 * It is installed after the module that provides IGameFlow, so its work runs
 * right after the rules in the same tick. start() adds its hook and its
 * handler, which stay registered after stop(): the game is destroyed right
 * after its modules stop, and neither is called in between.
 */
class ResultsModule final : public IGameModule {
 public:
  /** @param archive Keeps the results; it must outlive the module. */
  explicit ResultsModule(IResultsArchive& archive);

  [[nodiscard]] std::string name() const override;
  void install(GameContext& game, engine::InstallContext& install) override;
  void start() override;
  void stop() override;

 private:
  IResultsArchive& archive_;
  GameContext* game_{nullptr};
  IGameFlow* flow_{nullptr};
  std::unique_ptr<GameResultKeeper> keeper_;
  std::unique_ptr<LeaderboardDesk> desk_;
};

}  // namespace rtype::server

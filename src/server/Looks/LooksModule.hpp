#pragma once

#include <memory>
#include <string>
#include "IGameModule.hpp"
#include "InstallContext.hpp"
#include "LookRelay.hpp"

namespace rtype::server {

struct GameContext;

/**
 * @brief The module of a game that shares the look of each player with every
 * player. Always built: the look is part of a pilot.
 *
 * start() adds its functions to the hooks of the game, which stay there after
 * stop(): the game is destroyed right after its modules stop, and calls no
 * hook in between.
 */
class LooksModule final : public IGameModule {
 public:
  [[nodiscard]] std::string name() const override;
  void install(GameContext& game, engine::InstallContext& install) override;
  void start() override;
  void stop() override;

 private:
  GameContext* game_{nullptr};
  std::unique_ptr<LookRelay> relay_;
};

}  // namespace rtype::server

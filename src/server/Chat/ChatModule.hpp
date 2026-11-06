#pragma once

#include <memory>
#include <string>
#include "ChatRelay.hpp"
#include "IGameModule.hpp"
#include "InstallContext.hpp"

namespace rtype::server {

struct GameContext;

/**
 * @brief The module of a game that relays the chat lines of its players to all
 * of them, in the waiting room and during a match alike.
 *
 * Optional: nothing includes it but the list of the modules of a game, and a
 * game without it drops the lines its players send. start() adds its functions
 * to the hooks of the game, which stay there after stop(): the game is
 * destroyed right after its modules stop, and calls no hook in between.
 */
class ChatModule final : public IGameModule {
 public:
  [[nodiscard]] std::string name() const override;
  void install(GameContext& game, engine::InstallContext& install) override;
  void start() override;
  void stop() override;

 private:
  GameContext* game_{nullptr};
  std::unique_ptr<ChatRelay> relay_;
};

}  // namespace rtype::server

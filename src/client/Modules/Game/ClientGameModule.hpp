#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"

namespace rtype::client {

/**
 * @brief The module of a game in progress and of its end: the game screen and
 * the end screen.
 *
 * install() adds them to the ScreenRegistry under the identifiers of
 * GameScreenIdConstants.hpp. A game plays the music of a game, and loads the
 * assets of a game first when they are not. It opens nothing, so start() and
 * stop() do nothing.
 */
class ClientGameModule final : public engine::IModule {
 public:
  /** @returns GAME_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns The module that provides the registry. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException if no module provides the registry.
   * @throws DuplicateScreenException if another module registered one of the
   * identifiers. */
  void install(engine::InstallContext& context) override;
  void start() override;
  void stop() override;
};

}  // namespace rtype::client

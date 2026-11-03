#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"

namespace rtype::client {

/**
 * @brief The module of the screens a player meets outside a game: the intro,
 * the title, the options (over the title, and over a game), the quit
 * confirmation and the connection.
 *
 * install() adds them to the ScreenRegistry under the identifiers of
 * MenuScreenIdConstants.hpp. A title screen plays the theme, unless it plays
 * already, and stops the music of a game, once its assets are loaded. It opens
 * nothing, so start() and stop() do nothing.
 */
class ClientMenusModule final : public engine::IModule {
 public:
  /** @returns MENUS_MODULE_NAME. */
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

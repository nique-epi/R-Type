#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"

namespace rtype::client {

/**
 * @brief The module of the waiting room: the lobby, pushed over the
 * connection screen, and the lobby after a game, alone on the stack.
 *
 * install() adds them to the ScreenRegistry under the identifiers of
 * LobbyScreenIdConstants.hpp. A lobby after a game plays the theme again. It
 * opens nothing, so start() and stop() do nothing.
 */
class ClientLobbyModule final : public engine::IModule {
 public:
  /** @returns LOBBY_MODULE_NAME. */
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

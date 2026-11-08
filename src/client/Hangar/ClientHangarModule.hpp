#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"

namespace rtype::client {

/**
 * @brief The module of the hangar: install() adds HANGAR_SCREEN to the
 * ScreenRegistry, built with the ship color of the session when a server
 * accepted the player. It opens nothing, so start() and stop() do nothing.
 */
class ClientHangarModule final : public engine::IModule {
 public:
  /** @returns HANGAR_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns The module that provides the registry. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException if no module provides the registry.
   * @throws DuplicateScreenException if another module registered
   * HANGAR_SCREEN. */
  void install(engine::InstallContext& context) override;
  void start() override;
  void stop() override;
};

}  // namespace rtype::client

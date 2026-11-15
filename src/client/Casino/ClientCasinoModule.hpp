#pragma once

#include <string>
#include <vector>
#include "ICasinoChannel.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

/**
 * @brief The module of the casino: addWork() settles in the chips of the
 * player, each frame, what the rounds of the roulette table moved, whichever
 * screen shows. It opens nothing, so start() and stop() do nothing.
 */
class ClientCasinoModule final : public engine::IModule {
 public:
  /** @returns CASINO_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns The modules that provide the registry of the screens, the
   * table and the settings. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException if no module provides the registry, the
   * table or the settings. */
  void install(engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  ICasinoChannel* casino_{nullptr};
  SettingsService* settings_{nullptr};
};

}  // namespace rtype::client

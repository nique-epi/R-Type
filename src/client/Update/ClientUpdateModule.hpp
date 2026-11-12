#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "UpdateNotice.hpp"

namespace rtype::client {

/**
 * @brief The module that updates the client from the releases of the game,
 * through Velopack: it provides IUpdateNotice, which the title asks, and adds
 * the update panel and WHAT'S NEW to the ScreenRegistry. start() reads the
 * notes an update left and starts the check of the latest release; stop()
 * ends it, and an update in progress.
 */
class ClientUpdateModule final : public engine::IModule {
 public:
  ClientUpdateModule();

  /** @returns UPDATE_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns The module that provides the registry. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException if no module provides the registry. */
  void install(engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  UpdateNotice notice_;
};

}  // namespace rtype::client

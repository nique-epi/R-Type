#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "LatestBans.hpp"

/**
 * @brief A module that finds the LatestBans the storage module provides, so a
 * test publishes bans as the storage module would once it read them. It does
 * nothing else.
 */
class BanProbeModule : public rtype::engine::IModule {
 public:
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

  /** @returns The bans found at install; nullptr without a storage module. */
  [[nodiscard]] rtype::server::LatestBans* bans() const;

 private:
  rtype::server::LatestBans* bans_{nullptr};
};

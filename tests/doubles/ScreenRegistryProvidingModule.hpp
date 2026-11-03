#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "ScreenRegistry.hpp"

/** @brief Module double that takes the place of the screens module, whose
 * install() requires the window and other services: it provides, under the name
 * of that module, a screen registry the test owns. It opens nothing. */
class ScreenRegistryProvidingModule : public rtype::engine::IModule {
 public:
  /** @param registry What the module provides; it must outlive the module. */
  explicit ScreenRegistryProvidingModule(
      rtype::client::ScreenRegistry& registry);

  /** @returns SCREENS_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  rtype::client::ScreenRegistry* registry_;
};

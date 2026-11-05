#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "PluginHostState.hpp"

/** @brief Module double that provides, during its install(), the services a
 * test plugin requires: an EventBus, a DeliveredEvents and a PluginLifetime,
 * all three kept by the test. It opens nothing. */
class PluginHostModule : public rtype::engine::IModule {
 public:
  /** @param name The name the plugins depend on.
   * @param state What the module provides; it must outlive the module. */
  PluginHostModule(std::string name, PluginHostState& state);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  std::string name_;
  PluginHostState* state_;
};

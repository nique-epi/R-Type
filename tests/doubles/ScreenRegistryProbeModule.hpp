#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "ScreenRegistry.hpp"

/** @brief Module double that finds the screen registry during its install()
 * and hands it to the test. It opens nothing. */
class ScreenRegistryProbeModule : public rtype::engine::IModule {
 public:
  /** @param registry Where the registry found is written; it must outlive the
   * module. */
  explicit ScreenRegistryProbeModule(rtype::client::ScreenRegistry*& registry);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  rtype::client::ScreenRegistry** registry_;
};

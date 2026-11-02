#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "ScreenRegistry.hpp"

namespace rtype::client {

/**
 * @brief The module that provides the ScreenRegistry the modules of the
 * screens add theirs to, listed before them.
 *
 * install() provides the registry, empty. It opens nothing, so start() and
 * stop() do nothing.
 */
class ClientScreenRegistryModule final : public engine::IModule {
 public:
  /** @returns SCREEN_REGISTRY_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns No module: the registry needs no service. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  ScreenRegistry registry_;
};

}  // namespace rtype::client

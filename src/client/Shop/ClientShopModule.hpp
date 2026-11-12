#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"

namespace rtype::client {

/**
 * @brief The module of the shop: install() adds SHOP_SCREEN to the
 * ScreenRegistry. It opens nothing, so start() and stop() do nothing.
 */
class ClientShopModule final : public engine::IModule {
 public:
  /** @returns SHOP_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns The module that provides the registry. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException if no module provides the registry.
   * @throws DuplicateScreenException if another module registered
   * SHOP_SCREEN. */
  void install(engine::InstallContext& context) override;
  void start() override;
  void stop() override;
};

}  // namespace rtype::client

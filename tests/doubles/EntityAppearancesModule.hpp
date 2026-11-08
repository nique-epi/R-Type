#pragma once

#include <string>
#include <vector>
#include "EntityAppearances.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"

/**
 * @brief Stands in for the assets module of the client: it goes by
 * ASSETS_MODULE_NAME and provides an EntityAppearances that holds no sheet,
 * so a module that draws the entity types of the client installs without
 * loading a texture.
 */
class EntityAppearancesModule : public rtype::engine::IModule {
 public:
  EntityAppearancesModule();

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  rtype::client::EntityAppearances appearances_;
};

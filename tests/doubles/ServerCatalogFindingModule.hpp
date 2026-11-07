#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "IServerCatalog.hpp"
#include "InstallContext.hpp"

/**
 * @brief Listed after the connection module of the client, it requires the
 * IServerCatalog that module provides and writes where it is, so a test reads
 * the list of servers the server map would show.
 */
class ServerCatalogFindingModule : public rtype::engine::IModule {
 public:
  /** @param found Where the module writes the catalog it requires; it must
   * outlive the module. */
  explicit ServerCatalogFindingModule(rtype::client::IServerCatalog*& found);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  rtype::client::IServerCatalog** found_;
};

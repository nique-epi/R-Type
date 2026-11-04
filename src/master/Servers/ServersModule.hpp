#pragma once

#include <optional>
#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "ServersController.hpp"

namespace rtype::master {

/**
 * @brief The module of the game servers: their registration and their
 * heartbeats, `PUT /api/servers/me` and `PUT /api/servers/me/heartbeat`, kept
 * in the tables `servers` and `heartbeat_samples`.
 *
 * install() adds the routes to the MasterHttpServer; they read the database
 * only once the HTTP module listens, after the storage opened it.
 */
class ServersModule final : public engine::IModule {
 public:
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /** @brief Adds the routes of a game server; opens nothing. */
  void install(engine::InstallContext& context) override;

  void start() override;
  void stop() override;

 private:
  std::optional<ServersController> controller_;
};

}  // namespace rtype::master

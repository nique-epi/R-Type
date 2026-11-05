#pragma once

#include <optional>
#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "MasterDatabase.hpp"
#include "ServerStatusJob.hpp"
#include "ServersController.hpp"

namespace rtype::master {

/**
 * @brief The module of the game servers: their creation, their registration,
 * their heartbeats, their statuses and the lists of them, `POST /api/servers`,
 * `PUT /api/servers/me`, `PUT /api/servers/me/heartbeat`, `GET /api/servers`
 * and `GET /api/servers/{id}/games`, `/samples` and `/status-changes`, kept
 * in the tables `servers`, `heartbeat_samples` and `status_changes`.
 *
 * install() adds the routes to the MasterHttpServer; they read the database
 * only once the HTTP module listens, after the storage opened it. start()
 * starts the ServerStatusJob, on its thread StatusWatch.
 */
class ServersModule final : public engine::IModule {
 public:
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /** @brief Adds the routes of the game servers; opens nothing. */
  void install(engine::InstallContext& context) override;

  /** @brief Starts StatusWatch, which refreshes the statuses at once, then
   * every STATUS_REFRESH_INTERVAL. */
  void start() override;

  /** @brief Stops StatusWatch, once its refresh in progress is done. */
  void stop() override;

 private:
  MasterDatabase* database_{nullptr};
  std::optional<ServersController> controller_;
  std::optional<ServerStatusJob> statusJob_;
};

}  // namespace rtype::master

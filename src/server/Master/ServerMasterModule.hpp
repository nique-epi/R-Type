#pragma once

#include <optional>
#include <string>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "HeartbeatJob.hpp"
#include "HeartbeatJobParts.hpp"
#include "IListeningPort.hpp"
#include "IModule.hpp"
#include "IServerName.hpp"
#include "InstallContext.hpp"
#include "LatestHostedGames.hpp"
#include "LatestServerMeasures.hpp"
#include "Logger.hpp"
#include "SettingValue.hpp"

namespace rtype::server {

/**
 * @brief The module master of the supervisor, optional: it keeps the master
 * told of this server through a HeartbeatJob, from master-url, master-key and
 * the name of the information module. It registers the server once, with
 * MAXIMUM_RUNNING_GAMES games, and its heartbeats carry what the supervised
 * games module publishes: the games by place and their measures.
 *
 * Without master-url or master-key the server runs alone and MasterLink, the
 * thread of the job, does not start, as it does when master-url is not
 * `http://<host>[:<port>]` or the port the players use is chosen by the
 * system.
 */
class ServerMasterModule final : public engine::IModule {
 public:
  ServerMasterModule();

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /** @brief Declares the settings and reads the services; opens nothing. */
  void install(engine::InstallContext& context) override;

  /** @brief Starts the HeartbeatJob when the settings name a master. */
  void start() override;

  /** @brief Stops the HeartbeatJob, its call in progress included. */
  void stop() override;

 private:
  void declareSettings(engine::ConfigurationRegistry& configuration);
  /** @returns The parts of the job, or nothing when the server runs alone. */
  [[nodiscard]] std::optional<HeartbeatJobParts> jobParts() const;

  const engine::SettingValue* urlSetting_{nullptr};
  const engine::SettingValue* keySetting_{nullptr};
  const IServerName* serverName_{nullptr};
  const IListeningPort* port_{nullptr};
  const LatestHostedGames* hostedGames_{nullptr};
  const LatestServerMeasures* measures_{nullptr};
  logging::Logger logger_;
  std::optional<HeartbeatJob> job_;
};

}  // namespace rtype::server

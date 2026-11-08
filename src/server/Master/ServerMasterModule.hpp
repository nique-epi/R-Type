#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "HeartbeatJob.hpp"
#include "HeartbeatJobParts.hpp"
#include "IListeningPort.hpp"
#include "IModule.hpp"
#include "IResultsFile.hpp"
#include "IServerName.hpp"
#include "InstallContext.hpp"
#include "LatestHostedGames.hpp"
#include "LatestServerMeasures.hpp"
#include "LatestValue.hpp"
#include "Logger.hpp"
#include "MasterProbeDesk.hpp"
#include "SettingValue.hpp"
#include "SystemClock.hpp"
#include "TickDatagrams.hpp"

namespace rtype::server {

/**
 * @brief The module master of the supervisor, optional: it keeps the master
 * told of this server through a HeartbeatJob, from master-url, master-key and
 * the name of the information module. It registers the server once, with
 * MAXIMUM_RUNNING_GAMES games and the address public-address declares, if any
 * (the master takes the address of the request otherwise), and its heartbeats
 * carry what the supervised games module publishes: the games by place and
 * their measures. After each heartbeat, the job sends the vanilla games of the
 * file of results the supervised games module names (IResultsFile) that the
 * master has not answered.
 *
 * Without master-url or master-key the server runs alone and MasterLink, the
 * thread of the job, does not start and the file of results is never opened,
 * as when master-url is not
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
  /** @brief Keeps the token of the probes of this tick (SIMULATION_STAGE). */
  void keepProbeTokens();

  const engine::SettingValue* urlSetting_{nullptr};
  const engine::SettingValue* keySetting_{nullptr};
  const engine::SettingValue* publicAddressSetting_{nullptr};
  const IServerName* serverName_{nullptr};
  const IResultsFile* resultsFile_{nullptr};
  const IListeningPort* port_{nullptr};
  const LatestHostedGames* hostedGames_{nullptr};
  const LatestServerMeasures* measures_{nullptr};
  const TickDatagrams* tickDatagrams_{nullptr};
  engine::SystemClock clock_;
  std::uint64_t probeSecret_;
  concurrency::LatestValue<std::uint64_t> probeTokens_;
  MasterProbeDesk probeDesk_;
  logging::Logger logger_;
  std::optional<HeartbeatJob> job_;
};

}  // namespace rtype::server

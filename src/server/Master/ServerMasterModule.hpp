#pragma once

#include <optional>
#include <string>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "Games.hpp"
#include "HeartbeatJob.hpp"
#include "IListeningPort.hpp"
#include "IModule.hpp"
#include "IOptionalDependencies.hpp"
#include "InstallContext.hpp"
#include "LatestHostedGames.hpp"
#include "LatestMetrics.hpp"
#include "Logger.hpp"
#include "SettingValue.hpp"

namespace rtype::server {

/**
 * @brief The module master, optional: it keeps the master told of this server
 * through a HeartbeatJob, from master-url, master-key and server-name.
 *
 * Without master-url or master-key the server runs alone and no thread starts,
 * as it does when master-url is not `http://<host>[:<port>]` or the port the
 * players use is chosen by the system. The metrics module is optional too:
 * without it the heartbeats carry no measure.
 */
class ServerMasterModule final : public engine::IModule,
                                 public engine::IOptionalDependencies {
 public:
  ServerMasterModule();

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  [[nodiscard]] std::vector<std::string> optionalDependencies() const override;

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
  const engine::SettingValue* nameSetting_{nullptr};
  const IListeningPort* port_{nullptr};
  const Games* games_{nullptr};
  const LatestHostedGames* hostedGames_{nullptr};
  const LatestMetrics* metrics_{nullptr};
  logging::Logger logger_;
  std::optional<HeartbeatJob> job_;
};

}  // namespace rtype::server

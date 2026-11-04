#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "FailureReport.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "MasterHttpServer.hpp"
#include "SettingValue.hpp"

namespace rtype::master {

/**
 * @brief The module of the HTTP API of the master: it answers HEALTH_PATH,
 * and provides its MasterHttpServer so the modules listed after it add their
 * routes while they install.
 *
 * install() declares the `port` setting. start() listens on every IPv4
 * interface (network::ANY_IPV4_ADDRESS), as the server does, and on that port,
 * network::DEFAULT_MASTER_PORT until a `set` changes it.
 */
class MasterHttpModule final : public engine::IModule {
 public:
  /** @param onFailure Told when the HTTP server stops listening on its own. */
  explicit MasterHttpModule(FailureReport onFailure);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @brief Declares the `port` setting, adds the route of HEALTH_PATH and
   * provides the MasterHttpServer; opens nothing.
   */
  void install(engine::InstallContext& context) override;

  /**
   * @brief Listens on the port of the setting.
   * @throws HttpPortUnavailableException when another program holds it.
   */
  void start() override;

  /** @brief Stops listening, once the requests in progress are answered. */
  void stop() override;

 private:
  [[nodiscard]] std::uint16_t listeningPort() const;

  MasterHttpServer server_;
  const engine::SettingValue* portSetting_{nullptr};
  logging::Logger logger_;
};

}  // namespace rtype::master

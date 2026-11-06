#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "ProbeSender.hpp"

namespace rtype::master {

/**
 * @brief The module probe, required: it provides the IProbeSender the routes
 * of the game servers hand their probes to, and owns its socket and ProbeWire.
 *
 * install() provides the sender and opens nothing; start() opens the socket
 * and starts ProbeWire; stop() stops it, after the HTTP module stopped, since
 * the list of modules puts it before.
 */
class MasterProbeModule final : public engine::IModule {
 public:
  MasterProbeModule();

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /** @brief Provides the sender; opens nothing. */
  void install(engine::InstallContext& context) override;

  /**
   * @brief Opens the socket and starts ProbeWire.
   * @throws network::SocketOpenException when the socket cannot be opened.
   */
  void start() override;

  /** @brief Stops ProbeWire and closes the socket. */
  void stop() override;

 private:
  ProbeSender sender_;
  logging::Logger logger_;
};

}  // namespace rtype::master

#pragma once

#include <optional>
#include <string>
#include <vector>
#include "Endpoint.hpp"
#include "IModule.hpp"
#include "IStageRunner.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "ServerApplication.hpp"
#include "ServerLink.hpp"
#include "StopSignals.hpp"

namespace rtype::server {

/**
 * @brief The network of the server and the pace of its loop: the socket, the
 * network thread and the simulation thread of a ServerApplication, and the
 * stop signals.
 *
 * During its install() it provides the IServerLink the other modules send
 * through, and the datagrams of the current tick
 * (ServerApplication::ReceivedDatagrams, empty between two ticks), and it
 * requires the IStageRunner. Its start() opens the socket, then catches the
 * stop signals; its stop() gives the signals back, then closes the socket.
 *
 * Each tick of the simulation thread holds the datagrams received since the
 * previous tick, runs the stages of the loop with SIMULATION_TICK_DURATION,
 * then, when a stop signal arrived, makes run() return: the stop is seen at
 * the end of the first tick after the signal, at most one tick later.
 */
class NetworkModule : public engine::IModule {
 public:
  /**
   * @param localEndpoint Where the socket listens; port 0 lets the system pick
   * a free port.
   */
  explicit NetworkModule(network::Endpoint localEndpoint);

  /** @returns NETWORK_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns JOURNAL_MODULE_NAME: the log file is open before the socket. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /**
   * @throws MissingServiceException if no IStageRunner is provided, which an
   * Application always is.
   */
  void install(engine::InstallContext& context) override;
  /**
   * @throws InvalidAddressException, SocketOpenException when the socket
   * cannot be opened; StopSignalsRefusedException when the signals cannot be
   * caught, after the socket was closed again.
   */
  void start() override;
  void stop() override;

  /**
   * @brief Writes the port the server listens on, then runs the network on
   * the calling thread and the ticks on a thread of their own, until a stop
   * signal or until either thread fails. Call it once, after the start() of
   * every module returned, and stop the modules after it returned.
   * @throws The error that stopped either thread, such as an exception a stage
   * work threw.
   */
  void run();

 private:
  void runTick(const ServerApplication::ReceivedDatagrams& received);

  network::Endpoint localEndpoint_;
  ServerLink link_;
  ServerApplication::ReceivedDatagrams received_{};
  const engine::IStageRunner* stages_{nullptr};
  std::optional<StopSignals> stopSignals_;
  logging::Logger logger_;
};

}  // namespace rtype::server

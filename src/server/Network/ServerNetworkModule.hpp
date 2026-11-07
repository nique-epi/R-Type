#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "IListeningPort.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "NetworkConditions.hpp"
#include "ServerApplication.hpp"
#include "SettingValue.hpp"
#include "StopRequest.hpp"
#include "TickDatagrams.hpp"

namespace rtype::server {

/**
 * @brief The module that owns the two threads of the server and its UDP
 * socket. It provides to the modules installed after it the IDatagramSender,
 * the IPlayerAdmission, the TickDatagrams, the network::TrafficCounters of its
 * socket, the RejectionLedger of the datagrams its network thread rejects and
 * itself as the IListeningPort, and runs the stages of the loop once per
 * tick.
 *
 * install() builds the ServerApplication, which binds nothing, and declares the
 * `port` setting. start() binds the socket on the port the setting holds once
 * the configuration was read, and runs the server on a thread of its own;
 * stop() stops it, joins the thread and gives the port back. When the server
 * stops on an error, the module logs it and tells the StopRequest, which wakes
 * the thread that waits to stop the program.
 */
class ServerNetworkModule final : public engine::IModule,
                                  public IListeningPort {
 public:
  /** @param port The UDP port to listen on, on every IPv4 interface, unless
   * the configuration sets another; 0 lets the system pick one.
   * @param stopRequest Told when the server stops on an error; it must outlive
   * the module.
   * @param conditions The network the datagrams of the server cross, read at
   * launch. */
  ServerNetworkModule(std::uint16_t port, StopRequest& stopRequest,
                      const network::NetworkConditions& conditions);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @brief Builds the server, declares the `port` setting and provides the
   * services; binds nothing.
   * @throws MissingServiceException when the application is not provided.
   */
  void install(engine::InstallContext& context) override;

  /**
   * @brief Binds the socket, logs the port, and runs the server on a thread.
   * @throws SocketOpenException when the system refuses the port.
   */
  void start() override;

  /** @brief Stops the server, waits for its threads and closes the socket. */
  void stop() override;

  /** @returns The port the `port` setting holds, the one given at
   * construction until the configuration sets another. */
  [[nodiscard]] std::uint16_t listeningPort() const override;

 private:
  void runServer();

  std::uint16_t port_;
  const engine::SettingValue* portSetting_{nullptr};
  StopRequest* stopRequest_;
  network::NetworkConditions conditions_;
  logging::Logger logger_;
  TickDatagrams tickDatagrams_;
  std::unique_ptr<ServerApplication> server_;
  std::jthread serverThread_;
};

}  // namespace rtype::server

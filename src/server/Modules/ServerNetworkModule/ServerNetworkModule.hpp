#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "ServerApplication.hpp"
#include "StopRequest.hpp"
#include "TickDatagrams.hpp"

namespace rtype::server {

/**
 * @brief The module that owns the two threads of the server and its UDP
 * socket. It provides to the modules installed after it the IDatagramSender,
 * the IPlayerAdmission and the TickDatagrams, and runs the stages of the loop
 * once per tick.
 *
 * install() builds the ServerApplication, which binds nothing. start() binds
 * the socket on the port given to the constructor and runs the server on a
 * thread of its own; stop() stops it, joins the thread and gives the port
 * back. When the server stops on an error, the module logs it and tells the
 * StopRequest, which wakes the thread that waits to stop the program.
 */
class ServerNetworkModule final : public engine::IModule {
 public:
  /** @param port The UDP port to listen on, on every IPv4 interface; 0 lets
   * the system pick one.
   * @param stopRequest Told when the server stops on an error; it must outlive
   * the module. */
  ServerNetworkModule(std::uint16_t port, StopRequest& stopRequest);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @brief Builds the server and provides its services; binds nothing.
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

 private:
  void runServer();

  std::uint16_t port_;
  StopRequest* stopRequest_;
  logging::Logger logger_;
  TickDatagrams tickDatagrams_;
  std::optional<ServerApplication> server_;
  std::jthread serverThread_;
};

}  // namespace rtype::server

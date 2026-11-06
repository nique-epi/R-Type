#pragma once

#include <cstddef>
#include <future>
#include <memory>
#include <vector>
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IModule.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ServerGamesFixture.hpp"
#include "StopRequest.hpp"
#include "SupervisorLinkTestConstants.hpp"
#include "TimeConstants.hpp"

/** @brief The base of the tests of the supervisor link: the games fixture with
 * the link module listed after the games module, on a supervisor at
 * SUPERVISOR_PORT under LINK_TOKEN. */
class ServerSupervisorLinkFixture : public ServerGamesFixture {
 public:
  ServerSupervisorLinkFixture() = default;
  ~ServerSupervisorLinkFixture() override;

  ServerSupervisorLinkFixture(const ServerSupervisorLinkFixture&) = delete;
  ServerSupervisorLinkFixture& operator=(const ServerSupervisorLinkFixture&) =
      delete;
  ServerSupervisorLinkFixture(ServerSupervisorLinkFixture&&) = delete;
  ServerSupervisorLinkFixture& operator=(ServerSupervisorLinkFixture&&) =
      delete;

 protected:
  [[nodiscard]] std::vector<std::unique_ptr<rtype::engine::IModule>>
  modulesAfterTheGames() const override;

  /** @returns Where the supervisor sends from. */
  [[nodiscard]] static rtype::network::Endpoint supervisor();

  /** @returns A datagram of the supervisor that holds @p message, under
   * LINK_TOKEN. */
  template <typename Message>
  [[nodiscard]] static rtype::network::IncomingDatagram fromSupervisor(
      const Message& message) {
    return rtype::network::IncomingDatagram{
        .sender = supervisor(),
        .payload = rtype::network::DatagramCodec::encode(
            rtype::network::PacketHeader{.sessionToken = LINK_TOKEN}, message)};
  }

  /** @brief Runs one tick in which the supervisor sent @p message. */
  template <typename Message>
  void supervisorSays(const Message& message) {
    runTick({fromSupervisor(message)});
  }

  /** @brief Runs simulation ticks until @p duration has passed. */
  void runFor(rtype::engine::Duration duration);

  /** @returns Whether the program was asked to stop; waits a moment for it. */
  [[nodiscard]] bool stopWasRequested();

  /** @returns The GameStatus datagrams sent to the supervisor so far. */
  [[nodiscard]] std::vector<rtype::network::OutgoingDatagram> statuses();

 private:
  mutable rtype::server::StopRequest stopRequest_;
  std::vector<std::future<void>> waiting_;
};

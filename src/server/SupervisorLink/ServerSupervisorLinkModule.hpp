#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "Endpoint.hpp"
#include "GameProcessWatch.hpp"
#include "GameStatus.hpp"
#include "Games.hpp"
#include "IDatagramSender.hpp"
#include "IModule.hpp"
#include "IPlayerAdmission.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "StopRequest.hpp"
#include "SupervisorLink.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief The module of a game process that talks to its supervisor, in the
 * datagrams of the game's own socket.
 *
 * Every STATUS_INTERVAL it sends the supervisor a GameStatus: the state of its
 * game and its number of players, under the token the supervisor gave it. It
 * reads what the supervisor sends back: any datagram means the supervisor is
 * there, a Disconnection asks the process to stop. It asks the program to stop
 * (through the StopRequest, the way Ctrl-C does) when the supervisor orders it,
 * or when GameProcessWatch says so, so the games module says its goodbyes and
 * the process ends with the code 0.
 *
 * install() looks up the services and adds the work of the tick; start() admits
 * the supervisor, so the network lets its datagrams through; stop() forgets it.
 * Its work is in the simulation stage, after the games module's.
 */
class ServerSupervisorLinkModule final : public engine::IModule {
 public:
  /** @param link The supervisor to talk to and the token they share.
   * @param stopRequest Told to stop the program; it must outlive the module. */
  ServerSupervisorLinkModule(SupervisorLink link, StopRequest& stopRequest);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @throws MissingServiceException when no module provided the games, the
   * sender, the admission or the datagrams of the tick.
   */
  void install(engine::InstallContext& context) override;

  /** @brief Lets the datagrams of the supervisor through the network. */
  void start() override;

  /** @brief Stops letting them through. */
  void stop() override;

 private:
  void onTick(engine::Duration elapsed);
  void readSupervisorDatagrams();
  void sendStatusWhenDue(engine::Duration elapsed, std::size_t playerCount);
  [[nodiscard]] network::GameStatus currentStatus(
      std::size_t playerCount) const;
  void askToStop(std::string_view reason);

  SupervisorLink link_;
  network::Endpoint supervisor_;
  StopRequest* stopRequest_;
  logging::Logger logger_;
  GameProcessWatch watch_;
  engine::Duration sinceStatus_;
  network::GameStatus lastStatus_;
  std::uint16_t nextSequence_{0};
  bool stopAsked_{false};
  Games* games_{nullptr};
  IDatagramSender* sender_{nullptr};
  IPlayerAdmission* admission_{nullptr};
  TickDatagrams* tickDatagrams_{nullptr};
};

}  // namespace rtype::server

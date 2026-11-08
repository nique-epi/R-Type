#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "ConnectedPlayers.hpp"
#include "Endpoint.hpp"
#include "GameProcessWatch.hpp"
#include "GameStatus.hpp"
#include "IDatagramSender.hpp"
#include "IModule.hpp"
#include "IOptionalDependencies.hpp"
#include "IPlayerAdmission.hpp"
#include "InstallContext.hpp"
#include "LatestStatusMeasures.hpp"
#include "Logger.hpp"
#include "PlayerExpeller.hpp"
#include "RunningGame.hpp"
#include "StopRequest.hpp"
#include "SupervisorLink.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief The module of a game process that talks to its supervisor, in the
 * datagrams of the game's own socket.
 *
 * Every STATUS_INTERVAL, and at once when it changes, it sends the supervisor
 * a GameStatus: the state of its game, the nicknames of its connected players
 * and the measures of the newest report of the metrics module (see
 * LatestStatusMeasures), under the token the supervisor gave it. The metrics
 * module is optional: without it, the status carries no measure. It reads what
 * the supervisor sends back: any datagram means the supervisor is there, a
 * Disconnection asks the process to stop, and an ExpulsionOrder expels the
 * players it names (see PlayerExpeller). It asks the program to stop (through
 * the StopRequest, the way Ctrl-C does) when the supervisor orders it, or when
 * GameProcessWatch says so, so the games module says its goodbyes and the
 * process ends with the code 0.
 *
 * install() looks up the services and adds the work of the tick; start() admits
 * the supervisor, so the network lets its datagrams through; stop() forgets it.
 * Its work is in the simulation stage, after the games module's.
 */
class ServerSupervisorLinkModule final : public engine::IModule,
                                         public engine::IOptionalDependencies {
 public:
  /** @param link The supervisor to talk to and the token they share.
   * @param stopRequest Told to stop the program; it must outlive the module. */
  ServerSupervisorLinkModule(SupervisorLink link, StopRequest& stopRequest);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  [[nodiscard]] std::vector<std::string> optionalDependencies() const override;

  /**
   * @throws MissingServiceException when no module provided the game, the
   * sender, the admission or the datagrams of the tick. The metrics are
   * found, not required: without them the statuses carry no measure.
   */
  void install(engine::InstallContext& context) override;

  /** @brief Lets the datagrams of the supervisor through the network. */
  void start() override;

  /** @brief Stops letting them through. */
  void stop() override;

 private:
  void onTick(engine::Duration elapsed);
  void readSupervisorDatagrams();
  void sendStatusWhenDue(engine::Duration elapsed, network::GameStatus status,
                         const ConnectedPlayers& players);
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
  RunningGame* runningGame_{nullptr};
  LatestStatusMeasures measures_{nullptr};
  IDatagramSender* sender_{nullptr};
  IPlayerAdmission* admission_{nullptr};
  TickDatagrams* tickDatagrams_{nullptr};
  PlayerExpeller expeller_;
};

}  // namespace rtype::server

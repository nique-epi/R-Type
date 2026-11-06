#include "ServerSupervisorLinkModule.hpp"
#include <algorithm>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "GameInstance.hpp"
#include "GameState.hpp"
#include "GameStatus.hpp"
#include "IDatagramSender.hpp"
#include "IPlayerAdmission.hpp"
#include "IncomingDatagram.hpp"
#include "InstallContext.hpp"
#include "LatestMetrics.hpp"
#include "LatestStatusMeasures.hpp"
#include "MessageType.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "RunningGame.hpp"
#include "ServerConstants.hpp"
#include "StopRequest.hpp"
#include "SupervisorLink.hpp"
#include "SupervisorLinkConstants.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/** @returns The state of a game as the supervisor is told it: a game that is
 * starting no longer takes players, so it is in progress. */
network::GameStatusState statusStateOf(GameState state) {
  switch (state) {
    case GameState::WaitingForPlayers:
      return network::GameStatusState::WaitingForPlayers;
    case GameState::Starting:
    case GameState::InProgress:
      return network::GameStatusState::InProgress;
    case GameState::Finished:
      return network::GameStatusState::Finished;
  }
  return network::GameStatusState::WaitingForPlayers;
}

/** @returns Whether @p players are named @p nicknames, in that order. */
bool namedAs(const ConnectedPlayers& players,
             const std::vector<std::string>& nicknames) {
  return std::ranges::equal(
      players, nicknames, {},
      [](const ConnectedPlayer* player) -> const std::string& {
        return player->nickname;
      });
}

/** @returns Whether @p last, the status sent last, still tells the game: the
 * state and the measures of @p now, and the nicknames of @p players. */
bool stillTells(const network::GameStatus& last, const network::GameStatus& now,
                const ConnectedPlayers& players) {
  return last.state == now.state && last.measures == now.measures &&
         namedAs(players, last.nicknames);
}

/** @returns The status @p status, which holds the state and the measures,
 * with the nicknames of @p players. */
network::GameStatus namingPlayers(network::GameStatus status,
                                  const ConnectedPlayers& players) {
  status.nicknames.reserve(players.size());
  for (const ConnectedPlayer* const player : players) {
    status.nicknames.push_back(player->nickname);
  }
  return status;
}

/** @returns Whether @p datagram holds a Disconnection. */
bool holdsDisconnection(std::span<const std::byte> datagram) {
  const std::optional<network::DatagramCodec::MessageTypes> types =
      network::DatagramCodec::messageTypes(datagram);
  return types.has_value() && types->test(static_cast<std::size_t>(
                                  network::MessageType::Disconnection));
}

}  // namespace

ServerSupervisorLinkModule::ServerSupervisorLinkModule(SupervisorLink link,
                                                       StopRequest& stopRequest)
    : link_(link),
      supervisor_(network::Endpoint{
          .address = std::string{network::LOOPBACK_IPV4_ADDRESS},
          .port = link.port}),
      stopRequest_(&stopRequest),
      logger_(std::string{SERVER_LOGGER_NAME}),
      sinceStatus_(STATUS_INTERVAL) {}

std::string ServerSupervisorLinkModule::name() const {
  return std::string{SUPERVISOR_LINK_MODULE_NAME};
}

std::vector<std::string> ServerSupervisorLinkModule::dependencies() const {
  return {std::string{NETWORK_MODULE_NAME}, std::string{GAMES_MODULE_NAME}};
}

std::vector<std::string> ServerSupervisorLinkModule::optionalDependencies()
    const {
  return {std::string{METRICS_MODULE_NAME}};
}

void ServerSupervisorLinkModule::install(engine::InstallContext& context) {
  runningGame_ = &context.require<RunningGame>();
  sender_ = &context.require<IDatagramSender>();
  admission_ = &context.require<IPlayerAdmission>();
  tickDatagrams_ = &context.require<TickDatagrams>();
  measures_ = LatestStatusMeasures{context.find<LatestMetrics>()};
  context.addWork(SIMULATION_STAGE,
                  [this](engine::Duration elapsed) { onTick(elapsed); });
}

void ServerSupervisorLinkModule::start() {
  admission_->admit(supervisor_, link_.token);
}

void ServerSupervisorLinkModule::stop() {
  admission_->forget(supervisor_, link_.token);
}

void ServerSupervisorLinkModule::onTick(engine::Duration elapsed) {
  readSupervisorDatagrams();
  GameInstance& game = runningGame_->game();
  const ConnectedPlayers players = game.roster().connected();
  watch_.tick(elapsed, players.size());
  if (const std::optional<std::string_view> reason = watch_.reasonToStop()) {
    askToStop(*reason);
  }
  sendStatusWhenDue(elapsed,
                    network::GameStatus{.state = statusStateOf(game.state()),
                                        .nicknames = {},
                                        .measures = measures_.current()},
                    players);
}

void ServerSupervisorLinkModule::readSupervisorDatagrams() {
  for (const network::IncomingDatagram& datagram : tickDatagrams_->players()) {
    if (datagram.sender != supervisor_) {
      continue;
    }
    watch_.heardSupervisor();
    if (holdsDisconnection(datagram.payload)) {
      askToStop(SUPERVISOR_ORDER_REASON);
    }
  }
}

void ServerSupervisorLinkModule::sendStatusWhenDue(
    engine::Duration elapsed, network::GameStatus status,
    const ConnectedPlayers& players) {
  sinceStatus_ += elapsed;
  if (sinceStatus_ < STATUS_INTERVAL &&
      stillTells(lastStatus_, status, players)) {
    return;
  }
  sinceStatus_ = engine::Duration::zero();
  lastStatus_ = namingPlayers(std::move(status), players);
  sender_->queueDatagram(network::OutgoingDatagram{
      .destination = supervisor_,
      .payload = network::DatagramCodec::encode(
          network::PacketHeader{.sessionToken = link_.token,
                                .sequence = nextSequence_++},
          lastStatus_)});
}

void ServerSupervisorLinkModule::askToStop(std::string_view reason) {
  if (stopAsked_) {
    return;
  }
  stopAsked_ = true;
  logger_.info("stopping: ", reason);
  stopRequest_->request();
}

}  // namespace rtype::server

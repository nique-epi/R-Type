#include "ServerSupervisorLinkModule.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ConnectedPlayers.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "GameInstance.hpp"
#include "GameState.hpp"
#include "GameStatus.hpp"
#include "Games.hpp"
#include "IDatagramSender.hpp"
#include "IPlayerAdmission.hpp"
#include "IncomingDatagram.hpp"
#include "InstallContext.hpp"
#include "MessageType.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ServerConstants.hpp"
#include "StopRequest.hpp"
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

/** @returns How many players the only game of @p games holds. */
std::size_t playerCountOf(const Games& games) {
  const ConnectedPlayers players = games.all().front()->roster().connected();
  return players.size();
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

void ServerSupervisorLinkModule::install(engine::InstallContext& context) {
  games_ = &context.require<Games>();
  sender_ = &context.require<IDatagramSender>();
  admission_ = &context.require<IPlayerAdmission>();
  tickDatagrams_ = &context.require<TickDatagrams>();
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
  watch_.tick(elapsed, playerCountOf(*games_));
  if (const std::optional<std::string_view> reason = watch_.reasonToStop()) {
    askToStop(*reason);
  }
  sendStatusWhenDue(elapsed);
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

void ServerSupervisorLinkModule::sendStatusWhenDue(engine::Duration elapsed) {
  sinceStatus_ += elapsed;
  const network::GameStatus status = currentStatus();
  if (sinceStatus_ < STATUS_INTERVAL && status == lastStatus_) {
    return;
  }
  sinceStatus_ = engine::Duration::zero();
  lastStatus_ = status;
  sender_->queueDatagram(network::OutgoingDatagram{
      .destination = supervisor_,
      .payload = network::DatagramCodec::encode(
          network::PacketHeader{.sessionToken = link_.token,
                                .sequence = nextSequence_++},
          status)});
}

network::GameStatus ServerSupervisorLinkModule::currentStatus() const {
  const GameInstance& game = *games_->all().front();
  return network::GameStatus{
      .state = statusStateOf(game.state()),
      .playerCount = static_cast<std::uint8_t>(playerCountOf(*games_))};
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

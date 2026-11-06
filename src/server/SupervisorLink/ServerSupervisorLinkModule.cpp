#include "ServerSupervisorLinkModule.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <ratio>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "GameInstance.hpp"
#include "GameState.hpp"
#include "GameStatus.hpp"
#include "GameStatusMeasures.hpp"
#include "Games.hpp"
#include "IDatagramSender.hpp"
#include "IPlayerAdmission.hpp"
#include "IncomingDatagram.hpp"
#include "InstallContext.hpp"
#include "LatestMetrics.hpp"
#include "MessageType.hpp"
#include "MetricsMeasures.hpp"
#include "MetricsReport.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ServerConstants.hpp"
#include "ServerMeasures.hpp"
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

/** @returns @p milliseconds in whole microseconds, rounded to the nearest
 * and kept within the 32 bits of the wire; nothing for nothing. */
std::optional<std::uint32_t> microsecondsOf(
    std::optional<double> milliseconds) {
  if (!milliseconds.has_value()) {
    return std::nullopt;
  }
  const double microseconds =
      std::chrono::duration<double, std::micro>(
          std::chrono::duration<double, std::milli>(*milliseconds))
          .count();
  return static_cast<std::uint32_t>(std::clamp(
      std::round(microseconds), 0.0,
      static_cast<double>(std::numeric_limits<std::uint32_t>::max())));
}

/** @returns The measures of the newest report of @p metrics; none without the
 * metrics module, or before its first report. */
network::GameStatusMeasures measuresOf(const LatestMetrics* metrics) {
  if (metrics == nullptr) {
    return {};
  }
  const std::optional<MetricsReport> report = metrics->value();
  if (!report.has_value()) {
    return {};
  }
  const network::ServerMeasures measures = measuresOf(*report);
  return network::GameStatusMeasures{
      .tickMedianMicroseconds = microsecondsOf(measures.tickP50Milliseconds),
      .tickNinetyFifthPercentileMicroseconds =
          microsecondsOf(measures.tickP95Milliseconds),
      .averageRoundTripMicroseconds =
          microsecondsOf(measures.averagePingMilliseconds)};
}

/** @returns The status of @p game: its state, the nicknames of its connected
 * players, and the measures of @p metrics. */
network::GameStatus statusOf(GameInstance& game, const LatestMetrics* metrics) {
  network::GameStatus status{.state = statusStateOf(game.state()),
                             .nicknames = {},
                             .measures = measuresOf(metrics)};
  for (const ConnectedPlayer* const player : game.roster().connected()) {
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
  games_ = &context.require<Games>();
  sender_ = &context.require<IDatagramSender>();
  admission_ = &context.require<IPlayerAdmission>();
  tickDatagrams_ = &context.require<TickDatagrams>();
  metrics_ = context.find<LatestMetrics>();
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
  const network::GameStatus status = statusOf(*games_->all().front(), metrics_);
  watch_.tick(elapsed, status.nicknames.size());
  if (const std::optional<std::string_view> reason = watch_.reasonToStop()) {
    askToStop(*reason);
  }
  sendStatusWhenDue(elapsed, status);
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
    engine::Duration elapsed, const network::GameStatus& status) {
  sinceStatus_ += elapsed;
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

void ServerSupervisorLinkModule::askToStop(std::string_view reason) {
  if (stopAsked_) {
    return;
  }
  stopAsked_ = true;
  logger_.info("stopping: ", reason);
  stopRequest_->request();
}

}  // namespace rtype::server

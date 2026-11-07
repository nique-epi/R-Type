#include "ConnectionAcceptor.hpp"
#include <cstdint>
#include <exception>
#include <limits>
#include <optional>
#include <random>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "ConnectionAccepted.hpp"
#include "ConnectionAcceptorParts.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "LobbyConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "RequestReading.hpp"
#include "SessionDatagram.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/**
 * @returns Why a newcomer asking for @p request must be refused: a game is in
 * progress, the game is full, or the nickname is taken, checked in that order.
 * Nothing when the newcomer may join.
 */
std::optional<network::RefusalReason> refusalOf(
    const PlayerRoster& roster, const network::ConnectionRequest& request,
    bool gameInProgress) {
  if (gameInProgress) {
    return network::RefusalReason::GameInProgress;
  }
  if (roster.isFull()) {
    return network::RefusalReason::GameFull;
  }
  if (roster.hasNickname(request.nickname)) {
    return network::RefusalReason::NicknameTaken;
  }
  return std::nullopt;
}

/**
 * @returns Whether @p player, connected, may get its acceptance again at
 * @p now: it never got one, or CONNECTION_ACCEPTANCE_REPEAT_INTERVAL passed
 * since the previous one.
 */
bool isAcceptanceDue(const ConnectedPlayer& player, engine::Duration now) {
  return !player.lastAcceptedAt.has_value() ||
         now - *player.lastAcceptedAt >= CONNECTION_ACCEPTANCE_REPEAT_INTERVAL;
}

}  // namespace

ConnectionAcceptor::ConnectionAcceptor(ConnectionAcceptorParts parts)
    : roster_(&parts.roster.get()),
      send_(std::move(parts.send)),
      admit_(std::move(parts.admit)),
      onPlayerJoined_(std::move(parts.onPlayerJoined)),
      entityTypes_(std::move(parts.entityTypes)),
      shipGun_(parts.shipGun),
      logger_(std::string{LOBBY_LOGGER_NAME}) {}

void ConnectionAcceptor::receive(
    std::span<const network::IncomingDatagram> datagrams,
    engine::Duration now) {
  for (const network::IncomingDatagram& datagram : datagrams) {
    try {
      if (isOfAnotherVersion(datagram.payload)) {
        refuseAnotherVersion(datagram, now);
      } else if (const std::optional<network::ConnectionRequest> request =
                     requests_.read(datagram, now)) {
        answer(datagram.sender, *request, now);
      }
    } catch (const std::exception& error) {
      failureLog_.error(now, "dropped a datagram from ",
                        datagram.sender.address, " port ", datagram.sender.port,
                        ": answering it threw: ", error.what());
    }
  }
}

void ConnectionAcceptor::refuseAnotherVersion(
    const network::IncomingDatagram& datagram, engine::Duration now) {
  if (datagram.payload.size() < network::CONNECTION_REFUSED_DATAGRAM_SIZE) {
    requests_.drop(datagram.sender, SHORT_OTHER_VERSION_DROP_REASON, now);
    return;
  }
  refuse(datagram.sender, network::RefusalReason::IncompatibleVersion);
}

void ConnectionAcceptor::setGameInProgress(bool inProgress) {
  gameInProgress_ = inProgress;
}

void ConnectionAcceptor::answer(const network::Endpoint& sender,
                                const network::ConnectionRequest& request,
                                engine::Duration now) {
  const bool hasValidCookie = cookies_.isValid(request.cookie, sender, now);
  if (ConnectedPlayer* const connected = roster_->find(sender)) {
    if (!hasValidCookie) {
      challenge(sender, now);
    } else if (isAcceptanceDue(*connected, now)) {
      accept(*connected, now);
    }
    return;
  }
  if (const std::optional<network::RefusalReason> refusal =
          refusalOf(*roster_, request, gameInProgress_)) {
    refuse(sender, *refusal);
    return;
  }
  if (!hasValidCookie) {
    challenge(sender, now);
    return;
  }
  ConnectedPlayer* const joined =
      roster_->add(sender, request.nickname, drawSessionToken());
  logger_.info("player ", static_cast<unsigned int>(joined->playerNumber),
               " joined from ", sender.address, " port ", sender.port);
  admit_(joined->endpoint, joined->sessionToken);
  accept(*joined, now);
  if (onPlayerJoined_) {
    onPlayerJoined_(*joined);
  }
}

void ConnectionAcceptor::accept(ConnectedPlayer& player, engine::Duration now) {
  player.lastAcceptedAt = now;
  send_(sessionDatagram(
      player, network::ConnectionAccepted{.playerNumber = player.playerNumber,
                                          .types = entityTypes_,
                                          .shipGun = shipGun_}));
}

void ConnectionAcceptor::challenge(const network::Endpoint& sender,
                                   engine::Duration now) {
  logger_.debug("challenge sent to ", sender.address, " port ", sender.port);
  send_(network::OutgoingDatagram{
      .destination = sender,
      .payload = network::DatagramCodec::encode(
          network::PacketHeader{},
          network::ConnectionChallenge{.cookie =
                                           cookies_.issue(sender, now)})});
}

void ConnectionAcceptor::refuse(const network::Endpoint& sender,
                                network::RefusalReason reason) {
  logger_.debug("connection refused to ", sender.address, " port ", sender.port,
                ", reason ", static_cast<unsigned int>(reason));
  send_(network::OutgoingDatagram{
      .destination = sender,
      .payload = network::DatagramCodec::encode(
          network::PacketHeader{},
          network::ConnectionRefused{.reason = reason})});
}

std::uint64_t ConnectionAcceptor::drawSessionToken() {
  std::uniform_int_distribution<std::uint64_t> tokens{
      1, std::numeric_limits<std::uint64_t>::max()};
  return tokens(random_);
}

}  // namespace rtype::server

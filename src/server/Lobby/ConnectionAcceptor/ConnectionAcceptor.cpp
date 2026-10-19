#include "ConnectionAcceptor.hpp"
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <random>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "ConnectionAccepted.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "LobbyConstants.hpp"
#include "MessageFrame.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "SessionDatagram.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/**
 * @returns The first ConnectionRequest of @p datagram, or nothing when the
 * datagram is not of this protocol or holds no request.
 * @throws A NetworkException when the datagram is shorter than a header, its
 * messages do not split, or its first request is refused by its reader.
 */
std::optional<network::ConnectionRequest> firstConnectionRequest(
    std::span<const std::byte> datagram) {
  if (!network::DatagramCodec::speaksThisProtocol(
          network::DatagramCodec::decodeHeader(datagram))) {
    return std::nullopt;
  }
  for (const network::MessageFrame& frame :
       network::DatagramCodec::splitMessages(datagram)) {
    if (frame.type ==
        static_cast<std::uint8_t>(network::ConnectionRequest::TYPE)) {
      return network::decodeMessage<network::ConnectionRequest>(frame.payload);
    }
  }
  return std::nullopt;
}

}  // namespace

ConnectionAcceptor::ConnectionAcceptor(PlayerRoster& roster, Send send,
                                       Admit admit,
                                       OnPlayerJoined onPlayerJoined)
    : roster_(&roster),
      send_(std::move(send)),
      admit_(std::move(admit)),
      onPlayerJoined_(std::move(onPlayerJoined)),
      logger_(std::string{LOBBY_LOGGER_NAME}) {}

void ConnectionAcceptor::receive(
    std::span<const network::IncomingDatagram> datagrams,
    engine::Duration now) {
  for (const network::IncomingDatagram& datagram : datagrams) {
    if (const std::optional<network::ConnectionRequest> request =
            readRequest(datagram, now)) {
      answer(datagram.sender, *request, now);
    }
  }
}

void ConnectionAcceptor::setGameInProgress(bool inProgress) {
  gameInProgress_ = inProgress;
}

std::optional<network::ConnectionRequest> ConnectionAcceptor::readRequest(
    const network::IncomingDatagram& datagram, engine::Duration now) {
  std::optional<network::ConnectionRequest> request;
  try {
    request = firstConnectionRequest(datagram.payload);
  } catch (const network::NetworkException& error) {
    drop(datagram.sender, error.what(), now);
    return std::nullopt;
  }
  if (!request.has_value()) {
    drop(datagram.sender,
         "its header is not of this protocol and version, or it holds no "
         "connection request",
         now);
  }
  return request;
}

void ConnectionAcceptor::drop(const network::Endpoint& sender,
                              std::string_view reason, engine::Duration now) {
  dropLog_.debug(now, "dropped a datagram from ", sender.address, " port ",
                 sender.port, ": ", reason);
}

void ConnectionAcceptor::answer(const network::Endpoint& sender,
                                const network::ConnectionRequest& request,
                                engine::Duration now) {
  const bool hasValidCookie = cookies_.isValid(request.cookie, sender, now);
  if (ConnectedPlayer* const connected = roster_->find(sender)) {
    if (hasValidCookie) {
      accept(*connected);
    } else {
      challenge(sender, now);
    }
    return;
  }
  if (gameInProgress_) {
    refuse(sender, network::RefusalReason::GameInProgress);
    return;
  }
  if (roster_->isFull()) {
    refuse(sender, network::RefusalReason::GameFull);
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
  accept(*joined);
  if (onPlayerJoined_) {
    onPlayerJoined_(*joined);
  }
}

void ConnectionAcceptor::accept(ConnectedPlayer& player) {
  send_(sessionDatagram(player, network::ConnectionAccepted{
                                    .playerNumber = player.playerNumber}));
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

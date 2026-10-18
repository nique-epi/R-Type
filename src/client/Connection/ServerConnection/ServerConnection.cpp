#include "ServerConnection.hpp"
#include <string>
#include <string_view>
#include "ActionSet.hpp"
#include "ConnectionAccepted.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionConstants.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "ConnectionStatus.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IDatagramLink.hpp"
#include "IncomingDatagram.hpp"
#include "MessageOrigin.hpp"
#include "NetworkException.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "ProtocolConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ServerConnection::ServerConnection(IDatagramLink& link)
    : link_(&link), logger_(std::string{CONNECTION_LOGGER_NAME}) {
  registerAnswers();
}

void ServerConnection::connect(const network::Endpoint& server,
                               std::string_view nickname) {
  status_ = {};
  received_.clear();
  link_->drainReceived(received_);
  server_ = server;
  nickname_ = nickname;
  cookie_ = 0;
  sessionToken_ = 0;
  nextSequence_ = 0;
  nextInputNumber_ = network::FIRST_INPUT_NUMBER;
  sinceConnect_ = engine::Duration::zero();
  sendRequest();
  status_ = {.phase = ConnectionStatus::Phase::Connecting};
  logger_.info("connecting to ", server_.address, " port ", server_.port);
}

void ServerConnection::cancel() { status_ = {}; }

ConnectionStatus ServerConnection::status() const { return status_; }

void ServerConnection::update(engine::Duration elapsed) {
  received_.clear();
  link_->drainReceived(received_);
  for (const network::IncomingDatagram& datagram : received_) {
    receive(datagram);
  }
  if (!isConnecting()) {
    return;
  }
  sinceConnect_ += elapsed;
  if (sinceConnect_ >= CONNECTION_ATTEMPT_TIMEOUT) {
    status_ = {.phase = ConnectionStatus::Phase::Unreachable};
    logger_.warn("no answer from ", server_.address, " port ", server_.port);
    return;
  }
  sinceRequest_ += elapsed;
  if (sinceRequest_ >= CONNECTION_REQUEST_INTERVAL) {
    sendRequest();
  }
}

void ServerConnection::sendPlayerInput(ActionSet held) {
  if (status_.phase != ConnectionStatus::Phase::Accepted) {
    return;
  }
  link_->send(network::OutgoingDatagram{
      .destination = server_,
      .payload = network::DatagramCodec::encode(
          nextHeader(), network::PlayerInput{.inputNumber = nextInputNumber_,
                                             .actions = held.bits()})});
  ++nextInputNumber_;
}

void ServerConnection::registerAnswers() {
  dispatcher_.registerHandler<network::ConnectionChallenge>(
      [this](const network::MessageOrigin&,
             const network::ConnectionChallenge& challenge) {
        if (isConnecting()) {
          cookie_ = challenge.cookie;
          sendRequest();
        }
      });
  dispatcher_.registerHandler<network::ConnectionAccepted>(
      [this](const network::MessageOrigin& origin,
             const network::ConnectionAccepted& accepted) {
        if (isConnecting()) {
          sessionToken_ = origin.header.sessionToken;
          status_ = {.phase = ConnectionStatus::Phase::Accepted,
                     .playerNumber = accepted.playerNumber};
          logger_.info("accepted as player ",
                       static_cast<unsigned int>(accepted.playerNumber));
        }
      });
  dispatcher_.registerHandler<network::ConnectionRefused>(
      [this](const network::MessageOrigin&,
             const network::ConnectionRefused& refused) {
        if (isConnecting()) {
          status_ = {.phase = ConnectionStatus::Phase::Refused,
                     .refusal = refused.reason};
          logger_.info("refused, reason ",
                       static_cast<unsigned int>(refused.reason));
        }
      });
}

void ServerConnection::receive(const network::IncomingDatagram& datagram) {
  if (datagram.sender != server_) {
    return;
  }
  try {
    const network::PacketHeader header =
        network::DatagramCodec::decodeHeader(datagram.payload);
    if (!network::DatagramCodec::speaksThisProtocol(header)) {
      return;
    }
    dispatcher_.dispatch(
        {.sender = datagram.sender, .header = header},
        network::DatagramCodec::splitMessages(datagram.payload));
  } catch (const network::MalformedDatagramException&) {
    return;
  }
}

void ServerConnection::sendRequest() {
  link_->send(network::OutgoingDatagram{
      .destination = server_,
      .payload = network::DatagramCodec::encode(
          nextHeader(), network::ConnectionRequest{.cookie = cookie_,
                                                   .nickname = nickname_})});
  sinceRequest_ = engine::Duration::zero();
}

network::PacketHeader ServerConnection::nextHeader() {
  network::PacketHeader header;
  header.sessionToken = sessionToken_;
  header.sequence = nextSequence_;
  ++nextSequence_;
  return header;
}

bool ServerConnection::isConnecting() const {
  return status_.phase == ConnectionStatus::Phase::Connecting;
}

}  // namespace rtype::client

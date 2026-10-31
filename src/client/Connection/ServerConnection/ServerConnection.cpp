#include "ServerConnection.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ActionSet.hpp"
#include "ConnectionAccepted.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionConstants.hpp"
#include "ConnectionPhase.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "ConnectionStatus.hpp"
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "Endpoint.hpp"
#include "EntityDestroyed.hpp"
#include "EntityMessage.hpp"
#include "EntitySpawned.hpp"
#include "GameEnded.hpp"
#include "GameEvent.hpp"
#include "GameEventHistory.hpp"
#include "GameStarted.hpp"
#include "Heartbeat.hpp"
#include "IDatagramLink.hpp"
#include "IncomingDatagram.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "MessageType.hpp"
#include "NetworkException.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PendingDeparture.hpp"
#include "PlayerInput.hpp"
#include "PlayerLeft.hpp"
#include "ProtocolConstants.hpp"
#include "ReadyState.hpp"
#include "SequenceOrder.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::client {

namespace {

/** @brief Hands every message of these types that arrives to @p keep. */
template <typename... Messages, typename Keep>
void keepEachOf(network::MessageDispatcher& dispatcher, Keep keep) {
  (dispatcher.registerHandler<Messages>(
       [keep](const network::MessageOrigin&, const Messages& message) {
         keep(message);
       }),
   ...);
}

}  // namespace

ServerConnection::ServerConnection(IDatagramLink& link)
    : link_(&link), logger_(std::string{CONNECTION_LOGGER_NAME}) {
  registerAnswers();
  registerSessionMessages();
}

void ServerConnection::connect(const network::Endpoint& server,
                               std::string_view nickname) {
  cancel();
  received_.clear();
  link_->drainReceived(received_);
  server_ = server;
  nickname_ = nickname;
  cookie_ = 0;
  nextSequence_ = 0;
  nextInputNumber_ = network::FIRST_INPUT_NUMBER;
  sinceConnect_ = engine::Duration::zero();
  sinceReceived_ = engine::Duration::zero();
  sendRequest();
  status_ = {.phase = ConnectionPhase::Connecting};
  logger_.info("connecting to ", server_.address, " port ", server_.port);
}

void ServerConnection::cancel() {
  if (isAccepted()) {
    leave();
  }
  status_ = {};
  forgetSession();
}

ConnectionStatus ServerConnection::status() const { return status_; }

std::span<const network::LobbyPlayer> ServerConnection::lobbyPlayers() const {
  return lobbyPlayers_;
}

void ServerConnection::setReady(bool ready) {
  if (!isAccepted() || gameStarted_) {
    return;
  }
  requestedReady_ = ready;
  sendReadyState(ready);
}

std::optional<bool> ServerConnection::requestedReady() const {
  return requestedReady_;
}

bool ServerConnection::gameStarted() const { return gameStarted_; }

void ServerConnection::update(engine::Duration elapsed) {
  gameEvents_.advance(elapsed);
  sinceReceived_ += elapsed;
  readReceived();
  repeatDeparture(elapsed);
  if (isAccepted()) {
    holdSession(elapsed);
  } else if (isConnecting()) {
    continueAttempt(elapsed);
  }
}

void ServerConnection::readReceived() {
  received_.clear();
  link_->drainReceived(received_);
  for (const network::IncomingDatagram& datagram : received_) {
    receive(datagram);
  }
}

void ServerConnection::holdSession(engine::Duration elapsed) {
  if (sinceReceived_ >= network::SESSION_SILENCE_LIMIT) {
    giveUp(ConnectionPhase::Lost);
    return;
  }
  sinceSent_ += elapsed;
  repeatReadyState(elapsed);
  sendHeartbeatWhenSilent();
}

void ServerConnection::continueAttempt(engine::Duration elapsed) {
  sinceConnect_ += elapsed;
  if (sinceConnect_ >= CONNECTION_ATTEMPT_TIMEOUT) {
    giveUp(ConnectionPhase::Unreachable);
    return;
  }
  sinceRequest_ += elapsed;
  if (sinceRequest_ >= CONNECTION_REQUEST_INTERVAL) {
    sendRequest();
  }
}

void ServerConnection::sendPlayerInput(ActionSet held) {
  if (status_.phase != ConnectionPhase::Accepted) {
    return;
  }
  sendToServer(network::PlayerInput{.inputNumber = nextInputNumber_,
                                    .actions = held.bits()});
  ++nextInputNumber_;
}

void ServerConnection::drainEntityMessages(
    std::vector<EntityMessage>& destination) {
  std::ranges::move(entityMessages_, std::back_inserter(destination));
  entityMessages_.clear();
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
          status_ = {.phase = ConnectionPhase::Accepted,
                     .playerNumber = accepted.playerNumber};
          sessionToken_ = origin.header.sessionToken;
          newestReceivedSequence_ = origin.header.sequence;
          lobbyPlayers_ = {
              network::LobbyPlayer{.playerNumber = accepted.playerNumber,
                                   .ready = false,
                                   .nickname = nickname_}};
          logger_.info("accepted as player ",
                       static_cast<unsigned int>(accepted.playerNumber));
        }
      });
  dispatcher_.registerHandler<network::ConnectionRefused>(
      [this](const network::MessageOrigin& origin,
             const network::ConnectionRefused& refused) {
        if (isConnecting()) {
          status_ = {.phase = ConnectionPhase::Refused,
                     .refusal = refused.reason,
                     .serverVersion = origin.header.version};
          logger_.info("refused, reason ",
                       static_cast<unsigned int>(refused.reason));
        }
      });
}

void ServerConnection::registerSessionMessages() {
  dispatcher_.registerHandler<network::Heartbeat>(
      [](const network::MessageOrigin&, const network::Heartbeat&) {});
  dispatcher_.registerHandler<network::LobbyState>(
      [this](const network::MessageOrigin&, const network::LobbyState& lobby) {
        if (isAccepted()) {
          lobbyPlayers_ = lobby.players;
        }
      });
  dispatcher_.registerHandler<network::GameStarted>(
      [this](const network::MessageOrigin&, const network::GameStarted&) {
        startGame();
      });
  dispatcher_.registerHandler<network::WorldState>(
      [this](const network::MessageOrigin&, const network::WorldState& state) {
        startGame();
        keepEntityMessage(state);
      });
  keepEachOf<network::EntitySpawned, network::EntityDestroyed,
             network::PlayerLeft>(dispatcher_, [this](EntityMessage message) {
    keepEntityMessage(std::move(message));
  });
  dispatcher_.registerHandler<network::GameEvent>(
      [this](const network::MessageOrigin&, const network::GameEvent& event) {
        if (gameEvents_.isFirstCopy(event.eventNumber)) {
          keepEntityMessage(event);
        }
      });
  dispatcher_.registerHandler<network::GameEnded>(
      [this](const network::MessageOrigin&, const network::GameEnded& ended) {
        if (gameStarted_) {
          keepEntityMessage(ended);
          gameStarted_ = false;
          logger_.info("the game ended");
        }
      });
  dispatcher_.registerHandler<network::Disconnection>(
      [this](const network::MessageOrigin&, const network::Disconnection&) {
        if (isAccepted()) {
          giveUp(ConnectionPhase::Closed);
        }
      });
}

void ServerConnection::giveUp(ConnectionPhase ending) {
  status_ = {.phase = ending};
  forgetSession();
  if (ending == ConnectionPhase::Closed) {
    logger_.warn("closed by ", server_.address, " port ", server_.port);
    return;
  }
  const std::chrono::seconds silence = ending == ConnectionPhase::Lost
                                           ? network::SESSION_SILENCE_LIMIT
                                           : CONNECTION_ATTEMPT_TIMEOUT;
  logger_.warn("no answer from ", server_.address, " port ", server_.port,
               " for ", silence.count(), " s");
}

void ServerConnection::endAttemptForServerVersion(std::uint8_t serverVersion) {
  if (!isConnecting()) {
    return;
  }
  status_ = {.phase = ConnectionPhase::Refused,
             .refusal = network::RefusalReason::IncompatibleVersion,
             .serverVersion = serverVersion};
  logger_.warn("refused: the server speaks protocol version ",
               static_cast<unsigned int>(serverVersion), ", this client ",
               static_cast<unsigned int>(network::PROTOCOL_VERSION));
}

void ServerConnection::receive(const network::IncomingDatagram& datagram) {
  if (datagram.sender != server_) {
    return;
  }
  try {
    const network::PacketHeader header =
        network::DatagramCodec::decodeHeader(datagram.payload);
    if (network::DatagramCodec::speaksAnotherVersion(header)) {
      endAttemptForServerVersion(header.version);
      return;
    }
    if (!network::DatagramCodec::speaksThisProtocol(header) ||
        !passesSessionChecks(header)) {
      return;
    }
    const std::vector<network::MessageFrame> messages =
        network::DatagramCodec::splitMessages(datagram.payload);
    sinceReceived_ = engine::Duration::zero();
    dispatcher_.dispatch({.sender = datagram.sender, .header = header},
                         messages);
  } catch (const network::MalformedDatagramException&) {
    return;
  }
}

bool ServerConnection::passesSessionChecks(
    const network::PacketHeader& header) {
  if (!isAccepted()) {
    return true;
  }
  if (header.sessionToken != sessionToken_) {
    ++wrongTokenCount_;
    return false;
  }
  if (!network::isNewerSequence(header.sequence, newestReceivedSequence_)) {
    return false;
  }
  newestReceivedSequence_ = header.sequence;
  return true;
}

void ServerConnection::startGame() {
  if (isAccepted() && !gameStarted_) {
    gameStarted_ = true;
    requestedReady_.reset();
    logger_.info("the game started");
  }
}

void ServerConnection::keepEntityMessage(EntityMessage message) {
  if (gameStarted_) {
    entityMessages_.push_back(std::move(message));
  }
}

void ServerConnection::sendRequest() {
  sendToServer(
      network::ConnectionRequest{.cookie = cookie_, .nickname = nickname_});
  sinceRequest_ = engine::Duration::zero();
}

void ServerConnection::sendReadyState(bool ready) {
  sendToServer(network::ReadyState{.ready = ready});
  sinceReadyState_ = engine::Duration::zero();
}

void ServerConnection::repeatReadyState(engine::Duration elapsed) {
  if (!requestedReady_.has_value() || roomShowsReady(*requestedReady_)) {
    return;
  }
  sinceReadyState_ += elapsed;
  if (sinceReadyState_ >= READY_STATE_INTERVAL) {
    sendReadyState(*requestedReady_);
  }
}

void ServerConnection::sendHeartbeatWhenSilent() {
  if (sinceSent_ < network::HEARTBEAT_INTERVAL) {
    return;
  }
  sendToServer(network::Heartbeat{});
}

void ServerConnection::leave() {
  departure_ = PendingDeparture{.server = server_,
                                .sessionToken = sessionToken_,
                                .nextSequence = nextSequence_,
                                .copiesLeft = DISCONNECTION_COPY_COUNT,
                                .sinceCopy = engine::Duration::zero()};
  sendDepartureCopy();
  logger_.info("left ", server_.address, " port ", server_.port);
}

void ServerConnection::sendDepartureCopy() {
  if (!departure_.has_value()) {
    return;
  }
  link_->send(network::OutgoingDatagram{
      .destination = departure_->server,
      .payload = network::DatagramCodec::encode(
          headerOf(departure_->sessionToken, departure_->nextSequence),
          network::Disconnection{})});
  --departure_->copiesLeft;
  departure_->sinceCopy = engine::Duration::zero();
  if (departure_->copiesLeft <= 0) {
    departure_.reset();
  }
}

void ServerConnection::repeatDeparture(engine::Duration elapsed) {
  if (!departure_.has_value()) {
    return;
  }
  departure_->sinceCopy += elapsed;
  if (departure_->sinceCopy >= DISCONNECTION_COPY_INTERVAL) {
    sendDepartureCopy();
  }
}

void ServerConnection::forgetSession() {
  sessionToken_ = network::NO_SESSION_TOKEN;
  newestReceivedSequence_ = 0;
  lobbyPlayers_.clear();
  requestedReady_.reset();
  sinceReadyState_ = engine::Duration::zero();
  gameStarted_ = false;
  entityMessages_.clear();
  gameEvents_.forget();
}

void ServerConnection::sendToServer(network::MessageType type,
                                    std::span<const std::byte> payload) {
  link_->send(network::OutgoingDatagram{
      .destination = server_,
      .payload = network::DatagramCodec::encode(nextHeader(), type, payload)});
}

network::PacketHeader ServerConnection::nextHeader() {
  sinceSent_ = engine::Duration::zero();
  return headerOf(sessionToken_, nextSequence_);
}

network::PacketHeader ServerConnection::headerOf(std::uint64_t token,
                                                 std::uint16_t& sequence) {
  network::PacketHeader header;
  header.sessionToken = token;
  header.sequence = sequence;
  ++sequence;
  return header;
}

std::size_t ServerConnection::wrongTokenCount() const {
  return wrongTokenCount_;
}

bool ServerConnection::isLeaving() const { return departure_.has_value(); }

bool ServerConnection::isConnecting() const {
  return status_.phase == ConnectionPhase::Connecting;
}

bool ServerConnection::isAccepted() const {
  return status_.phase == ConnectionPhase::Accepted;
}

bool ServerConnection::roomShowsReady(bool ready) const {
  const auto own = std::ranges::find(lobbyPlayers_, status_.playerNumber,
                                     &network::LobbyPlayer::playerNumber);
  return own != lobbyPlayers_.end() && own->ready == ready;
}

}  // namespace rtype::client

#include "ServerSession.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "ActionSet.hpp"
#include "AttemptProgress.hpp"
#include "ConnectionConstants.hpp"
#include "ConnectionPhase.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionStatus.hpp"
#include "Disconnection.hpp"
#include "Endpoint.hpp"
#include "HandshakeAnswersParts.hpp"
#include "ISessionOutbox.hpp"
#include "IncomingDatagram.hpp"
#include "Logger.hpp"
#include "MessageOrigin.hpp"
#include "PlayerInput.hpp"
#include "ProtocolConstants.hpp"
#include "Reception.hpp"
#include "ReceptionKind.hpp"
#include "ServerSessionParts.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The status of an attempt the server of @p serverVersion refuses,
 * and logs both versions. */
ConnectionStatus refusedForVersion(logging::Logger& logger,
                                   std::uint8_t serverVersion) {
  logger.warn("refused: the server speaks protocol version ",
              static_cast<unsigned int>(serverVersion), ", this client ",
              static_cast<unsigned int>(network::PROTOCOL_VERSION));
  return ConnectionStatus{
      .phase = ConnectionPhase::Refused,
      .refusal = network::RefusalReason::IncompatibleVersion,
      .serverVersion = serverVersion};
}

}  // namespace

ServerSession::ServerSession(ServerSessionParts parts)
    : outbox_(parts.link.get()),
      inbound_(parts.link.get()),
      observers_(std::move(parts.observers)),
      dispatcher_(&parts.dispatcher.get()),
      gameLoaded_(&parts.gameLoaded.get()),
      logger_(std::string{CONNECTION_LOGGER_NAME}),
      answers_(HandshakeAnswersParts{.dispatcher = parts.dispatcher,
                                     .status = status_,
                                     .attempt = attempt_,
                                     .outbox = outbox_,
                                     .inbound = inbound_,
                                     .observers = observers_,
                                     .logger = logger_}) {
  dispatcher_->registerHandler<network::Disconnection>(
      [this](const network::MessageOrigin&, const network::Disconnection&) {
        if (status_.phase == ConnectionPhase::Accepted) {
          giveUp(ConnectionPhase::Closed);
        }
      });
}

void ServerSession::connect(const network::Endpoint& server,
                            std::string_view nickname) {
  cancel();
  inbound_.listenTo(server);
  outbox_.aim(server);
  attempt_.begin(nickname);
  nextInputNumber_ = network::FIRST_INPUT_NUMBER;
  attempt_.sendRequest(outbox_);
  status_ = {.phase = ConnectionPhase::Connecting};
  logger_.info("connecting to ", server.address, " port ", server.port);
}

void ServerSession::cancel() {
  if (status_.phase == ConnectionPhase::Accepted) {
    departure_.leave(outbox_);
  }
  status_ = {};
  forgetSession();
}

ConnectionStatus ServerSession::status() const { return status_; }

void ServerSession::update(engine::Duration elapsed) {
  inbound_.advance(elapsed);
  readReceived();
  departure_.update(elapsed);
  if (status_.phase == ConnectionPhase::Accepted) {
    if (inbound_.isSilentTooLong()) {
      giveUp(ConnectionPhase::Lost);
      return;
    }
    outbox_.advance(elapsed);
    observers_.held(elapsed);
    outbox_.sendHeartbeatWhenSilent();
  } else if (status_.phase == ConnectionPhase::Connecting &&
             attempt_.advance(elapsed, outbox_) == AttemptProgress::TimedOut) {
    giveUp(ConnectionPhase::Unreachable);
  }
}

std::optional<std::uint32_t> ServerSession::sendPlayerInput(ActionSet held) {
  if (status_.phase != ConnectionPhase::Accepted) {
    return std::nullopt;
  }
  gameLoaded_->sendWhenDue(outbox_);
  const std::uint32_t number = nextInputNumber_;
  sendMessage(outbox_, network::PlayerInput{.inputNumber = number,
                                            .actions = held.bits()});
  ++nextInputNumber_;
  return number;
}

bool ServerSession::isLeaving() const { return departure_.isLeaving(); }

void ServerSession::readReceived() {
  for (const network::IncomingDatagram& datagram : inbound_.drain()) {
    const Reception reception = inbound_.receive(datagram);
    if (reception.kind == ReceptionKind::Read) {
      dispatcher_->dispatch({.sender = datagram.sender,
                             .header = reception.header,
                             .receivedAt = datagram.receivedAt},
                            reception.messages);
    } else if (reception.kind == ReceptionKind::AnotherVersion &&
               status_.phase == ConnectionPhase::Connecting) {
      status_ = refusedForVersion(logger_, reception.header.version);
    }
  }
}

void ServerSession::giveUp(ConnectionPhase ending) {
  status_ = {.phase = ending};
  forgetSession();
  if (ending == ConnectionPhase::Closed) {
    logger_.warn("closed by ", outbox_.server().address, " port ",
                 outbox_.server().port);
    return;
  }
  const std::chrono::seconds silence = ending == ConnectionPhase::Lost
                                           ? network::SESSION_SILENCE_LIMIT
                                           : CONNECTION_ATTEMPT_TIMEOUT;
  logger_.warn("no answer from ", outbox_.server().address, " port ",
               outbox_.server().port, " for ", silence.count(), " s");
}

void ServerSession::forgetSession() {
  outbox_.closeSession();
  inbound_.close();
  observers_.closed();
}

}  // namespace rtype::client

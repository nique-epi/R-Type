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
#include "Expulsion.hpp"
#include "GameRequest.hpp"
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

/** @returns Whether a session holds a seat on the server in @p phase: once
 * accepted, while its content loads too. */
bool holdsASession(ConnectionPhase phase) {
  return phase == ConnectionPhase::Accepted ||
         phase == ConnectionPhase::LoadingContent;
}

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
    : acknowledgements_(parts.arrivals.get()),
      outbox_(parts.link.get(), acknowledgements_),
      inbound_(parts.link.get(), acknowledgements_),
      observers_(std::move(parts.observers)),
      dispatcher_(&parts.dispatcher.get()),
      gameLoaded_(&parts.gameLoaded.get()),
      entityTypes_(&parts.entityTypes.get()),
      content_(&parts.content.get()),
      logger_(std::string{CONNECTION_LOGGER_NAME}),
      answers_(HandshakeAnswersParts{.dispatcher = parts.dispatcher,
                                     .status = status_,
                                     .attempt = attempt_,
                                     .outbox = outbox_,
                                     .inbound = inbound_,
                                     .observers = observers_,
                                     .departure = departure_,
                                     .entityTypes = parts.entityTypes,
                                     .content = parts.content,
                                     .logger = logger_}) {
  dispatcher_->registerHandler<network::Disconnection>(
      [this](const network::MessageOrigin&, const network::Disconnection&) {
        if (holdsASession(status_.phase)) {
          giveUp(ConnectionPhase::Closed);
        }
      });
  dispatcher_->registerHandler<network::Expulsion>(
      [this](const network::MessageOrigin&, const network::Expulsion&) {
        if (holdsASession(status_.phase)) {
          giveUp(ConnectionPhase::Expelled);
        }
      });
}

void ServerSession::connect(const network::Endpoint& server,
                            std::string_view nickname,
                            const GameRequest& request) {
  cancel();
  inbound_.listenTo(server);
  outbox_.aim(server);
  attempt_.begin(nickname, request);
  unconfirmedInputs_.clear();
  attempt_.sendRequest(outbox_);
  status_ = {.phase = ConnectionPhase::Connecting};
  logger_.info("connecting to ", server.address, " port ", server.port);
}

void ServerSession::cancel() {
  if (holdsASession(status_.phase)) {
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
  if (holdsASession(status_.phase)) {
    if (inbound_.isSilentTooLong()) {
      giveUp(ConnectionPhase::Lost);
      return;
    }
    outbox_.advance(elapsed);
    observers_.held(elapsed);
    if (status_.phase == ConnectionPhase::LoadingContent &&
        answers_.followContent()) {
      departure_.leave(outbox_);
      forgetSession();
    }
  } else if (status_.phase == ConnectionPhase::Connecting &&
             attempt_.advance(elapsed, outbox_) == AttemptProgress::TimedOut) {
    giveUp(ConnectionPhase::Unreachable);
  }
}

std::optional<std::uint32_t> ServerSession::sendPlayerInput(
    ActionSet held, std::uint32_t shownTick) {
  if (status_.phase != ConnectionPhase::Accepted) {
    return std::nullopt;
  }
  gameLoaded_->sendWhenDue(outbox_);
  const network::PlayerInput input =
      unconfirmedInputs_.add(held.bits(), shownTick);
  sendMessage(outbox_, input);
  return input.inputNumber;
}

void ServerSession::confirmInputsUpTo(std::uint32_t acknowledged) {
  unconfirmedInputs_.confirmUpTo(acknowledged);
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
  if (ending == ConnectionPhase::Closed ||
      ending == ConnectionPhase::Expelled) {
    logger_.warn(
        ending == ConnectionPhase::Expelled ? "expelled by " : "closed by ",
        outbox_.server().address, " port ", outbox_.server().port);
    return;
  }
  const std::chrono::seconds silence =
      ending == ConnectionPhase::Lost ? network::SESSION_SILENCE_LIMIT
                                      : network::CONNECTION_ATTEMPT_TIMEOUT;
  logger_.warn("no answer from ", outbox_.server().address, " port ",
               outbox_.server().port, " for ", silence.count(), " s");
}

void ServerSession::forgetSession() {
  outbox_.closeSession();
  inbound_.close();
  entityTypes_->forget();
  content_->cancel();
  observers_.closed();
}

}  // namespace rtype::client

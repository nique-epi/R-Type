#include "ServerSession.hpp"
#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include "ActionSet.hpp"
#include "AttemptProgress.hpp"
#include "ConnectionAccepted.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionConstants.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "Heartbeat.hpp"
#include "ISessionOutbox.hpp"
#include "IncomingDatagram.hpp"
#include "LobbyState.hpp"
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
      .phase = ConnectionStatus::Phase::Refused,
      .refusal = network::RefusalReason::IncompatibleVersion,
      .serverVersion = serverVersion};
}

}  // namespace

ServerSession::ServerSession(ServerSessionParts parts)
    : outbox_(parts.link.get()),
      inbound_(parts.link.get()),
      observers_(std::move(parts.observers)),
      dispatcher_(&parts.dispatcher.get()),
      logger_(std::string{CONNECTION_LOGGER_NAME}) {
  registerHandshake();
}

void ServerSession::registerHandshake() {
  dispatcher_->registerHandler<network::ConnectionChallenge>(
      [this](const network::MessageOrigin&,
             const network::ConnectionChallenge& challenge) {
        if (status_.phase == ConnectionStatus::Phase::Connecting) {
          attempt_.answerChallenge(challenge.cookie, outbox_);
        }
      });
  dispatcher_->registerHandler<network::ConnectionAccepted>(
      [this](const network::MessageOrigin& origin,
             const network::ConnectionAccepted& accepted) {
        if (status_.phase != ConnectionStatus::Phase::Connecting) {
          return;
        }
        status_ = {.phase = ConnectionStatus::Phase::Accepted,
                   .playerNumber = accepted.playerNumber};
        outbox_.openSession(origin.header.sessionToken);
        inbound_.open(origin.header);
        observers_.opened(
            network::LobbyPlayer{.playerNumber = accepted.playerNumber,
                                 .ready = false,
                                 .nickname = attempt_.nickname()},
            outbox_);
        logger_.info("accepted as player ",
                     static_cast<unsigned int>(accepted.playerNumber));
      });
  dispatcher_->registerHandler<network::ConnectionRefused>(
      [this](const network::MessageOrigin& origin,
             const network::ConnectionRefused& refused) {
        if (status_.phase == ConnectionStatus::Phase::Connecting) {
          status_ = {.phase = ConnectionStatus::Phase::Refused,
                     .refusal = refused.reason,
                     .serverVersion = origin.header.version};
          logger_.info("refused, reason ",
                       static_cast<unsigned int>(refused.reason));
        }
      });
  dispatcher_->registerHandler<network::Heartbeat>(
      [](const network::MessageOrigin&, const network::Heartbeat&) {});
}

void ServerSession::connect(const network::Endpoint& server,
                            std::string_view nickname) {
  cancel();
  inbound_.listenTo(server);
  outbox_.aim(server);
  attempt_.begin(nickname);
  nextInputNumber_ = network::FIRST_INPUT_NUMBER;
  attempt_.sendRequest(outbox_);
  status_ = {.phase = ConnectionStatus::Phase::Connecting};
  logger_.info("connecting to ", server.address, " port ", server.port);
}

void ServerSession::cancel() {
  if (status_.phase == ConnectionStatus::Phase::Accepted) {
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
  if (status_.phase == ConnectionStatus::Phase::Accepted) {
    if (inbound_.isSilentTooLong()) {
      giveUp(ConnectionStatus::Phase::Lost);
      return;
    }
    outbox_.advance(elapsed);
    observers_.held(elapsed);
    outbox_.sendHeartbeatWhenSilent();
  } else if (status_.phase == ConnectionStatus::Phase::Connecting &&
             attempt_.advance(elapsed, outbox_) == AttemptProgress::TimedOut) {
    giveUp(ConnectionStatus::Phase::Unreachable);
  }
}

void ServerSession::sendPlayerInput(ActionSet held) {
  if (status_.phase != ConnectionStatus::Phase::Accepted) {
    return;
  }
  sendMessage(outbox_, network::PlayerInput{.inputNumber = nextInputNumber_,
                                            .actions = held.bits()});
  ++nextInputNumber_;
}

bool ServerSession::isLeaving() const { return departure_.isLeaving(); }

void ServerSession::readReceived() {
  for (const network::IncomingDatagram& datagram : inbound_.drain()) {
    const Reception reception = inbound_.receive(datagram);
    if (reception.kind == ReceptionKind::Read) {
      dispatcher_->dispatch(
          {.sender = datagram.sender, .header = reception.header},
          reception.messages);
    } else if (reception.kind == ReceptionKind::AnotherVersion &&
               status_.phase == ConnectionStatus::Phase::Connecting) {
      status_ = refusedForVersion(logger_, reception.header.version);
    }
  }
}

void ServerSession::giveUp(ConnectionStatus::Phase ending) {
  const std::chrono::seconds silence = ending == ConnectionStatus::Phase::Lost
                                           ? network::SESSION_SILENCE_LIMIT
                                           : CONNECTION_ATTEMPT_TIMEOUT;
  status_ = {.phase = ending};
  forgetSession();
  logger_.warn("no answer from ", outbox_.server().address, " port ",
               outbox_.server().port, " for ", silence.count(), " s");
}

void ServerSession::forgetSession() {
  outbox_.closeSession();
  inbound_.close();
  observers_.closed();
}

}  // namespace rtype::client

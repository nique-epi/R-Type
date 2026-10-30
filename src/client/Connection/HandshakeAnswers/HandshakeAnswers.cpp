#include "HandshakeAnswers.hpp"
#include "ConnectionAccepted.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionStatus.hpp"
#include "HandshakeAnswersParts.hpp"
#include "Heartbeat.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"

namespace rtype::client {

HandshakeAnswers::HandshakeAnswers(HandshakeAnswersParts parts)
    : parts_(parts) {
  network::MessageDispatcher& dispatcher = parts_.dispatcher.get();
  dispatcher.registerHandler<network::ConnectionChallenge>(
      [this](const network::MessageOrigin&,
             const network::ConnectionChallenge& challenge) {
        onChallenge(challenge);
      });
  dispatcher.registerHandler<network::ConnectionAccepted>(
      [this](const network::MessageOrigin& origin,
             const network::ConnectionAccepted& accepted) {
        onAccepted(origin, accepted);
      });
  dispatcher.registerHandler<network::ConnectionRefused>(
      [this](const network::MessageOrigin& origin,
             const network::ConnectionRefused& refused) {
        onRefused(origin, refused);
      });
  dispatcher.registerHandler<network::Heartbeat>(
      [](const network::MessageOrigin&, const network::Heartbeat&) {});
}

bool HandshakeAnswers::isConnecting() const {
  return parts_.status.get().phase == ConnectionStatus::Phase::Connecting;
}

void HandshakeAnswers::onChallenge(
    const network::ConnectionChallenge& challenge) {
  if (isConnecting()) {
    parts_.attempt.get().answerChallenge(challenge.cookie, parts_.outbox.get());
  }
}

void HandshakeAnswers::onAccepted(const network::MessageOrigin& origin,
                                  const network::ConnectionAccepted& accepted) {
  if (!isConnecting()) {
    return;
  }
  parts_.status.get() = {.phase = ConnectionStatus::Phase::Accepted,
                         .playerNumber = accepted.playerNumber};
  parts_.outbox.get().openSession(origin.header.sessionToken);
  parts_.inbound.get().open(origin.header);
  parts_.observers.get().opened(
      network::LobbyPlayer{.playerNumber = accepted.playerNumber,
                           .ready = false,
                           .nickname = parts_.attempt.get().nickname()},
      parts_.outbox.get());
  parts_.logger.get().info("accepted as player ",
                           static_cast<unsigned int>(accepted.playerNumber));
}

void HandshakeAnswers::onRefused(const network::MessageOrigin& origin,
                                 const network::ConnectionRefused& refused) {
  if (!isConnecting()) {
    return;
  }
  parts_.status.get() = {.phase = ConnectionStatus::Phase::Refused,
                         .refusal = refused.reason,
                         .serverVersion = origin.header.version};
  parts_.logger.get().info("refused, reason ",
                           static_cast<unsigned int>(refused.reason));
}

}  // namespace rtype::client

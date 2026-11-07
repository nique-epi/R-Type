#include "HandshakeAnswers.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include "ConnectionAccepted.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionPhase.hpp"
#include "ConnectionRedirect.hpp"
#include "ConnectionRefused.hpp"
#include "Endpoint.hpp"
#include "ErrorText.hpp"
#include "HandshakeAnswersParts.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "SessionOutbox.hpp"

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
  dispatcher.registerHandler<network::ConnectionRedirect>(
      [this](const network::MessageOrigin&,
             const network::ConnectionRedirect& redirect) {
        onRedirected(redirect);
      });
}

bool HandshakeAnswers::isConnecting() const {
  return parts_.status.get().phase == ConnectionPhase::Connecting;
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
  const std::vector<std::string> undrawable =
      parts_.entityTypes.get().undrawableNamesOf(accepted.types);
  if (undrawable.empty()) {
    openSession(origin, accepted);
  } else {
    leaveForMissingSheets(origin, undrawable);
  }
}

void HandshakeAnswers::openSession(
    const network::MessageOrigin& origin,
    const network::ConnectionAccepted& accepted) const {
  parts_.status.get() = {.phase = ConnectionPhase::Accepted,
                         .playerNumber = accepted.playerNumber};
  parts_.entityTypes.get().keep(accepted.types, accepted.shipGun);
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

void HandshakeAnswers::leaveForMissingSheets(
    const network::MessageOrigin& origin,
    const std::vector<std::string>& undrawable) const {
  parts_.outbox.get().openSession(origin.header.sessionToken);
  parts_.departure.get().leave(parts_.outbox.get());
  parts_.outbox.get().closeSession();
  parts_.status.get() = {
      .phase = ConnectionPhase::MissingEntitySheet,
      .missingSheet = undrawable.front(),
      .otherMissingSheets = static_cast<std::uint8_t>(undrawable.size() - 1)};
  parts_.logger.get().warn(
      "accepted, but this client has no sheet for the entity types ",
      engine::listText(undrawable), ": it leaves the server");
}

void HandshakeAnswers::onRedirected(
    const network::ConnectionRedirect& redirect) {
  SessionOutbox& outbox = parts_.outbox.get();
  const network::Endpoint game{.address = outbox.server().address,
                               .port = redirect.port};
  if (!isConnecting() || game == outbox.server() ||
      !parts_.attempt.get().acceptRedirect()) {
    return;
  }
  parts_.inbound.get().listenTo(game);
  outbox.aim(game);
  parts_.attempt.get().answerRedirect(outbox);
  parts_.logger.get().info("redirected to port ", redirect.port);
}

void HandshakeAnswers::onRefused(const network::MessageOrigin& origin,
                                 const network::ConnectionRefused& refused) {
  if (!isConnecting()) {
    return;
  }
  parts_.status.get() = {.phase = ConnectionPhase::Refused,
                         .refusal = refused.reason,
                         .serverVersion = origin.header.version};
  parts_.logger.get().info("refused, reason ",
                           static_cast<unsigned int>(refused.reason));
}

}  // namespace rtype::client

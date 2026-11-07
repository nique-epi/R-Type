#include "HandshakeAnswers.hpp"
#include <optional>
#include <utility>
#include "ConnectionAccepted.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionPhase.hpp"
#include "ConnectionRedirect.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionStatus.hpp"
#include "ContentFingerprintText.hpp"
#include "ContentLoading.hpp"
#include "ContentStep.hpp"
#include "Endpoint.hpp"
#include "HandshakeAnswersParts.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "PackContent.hpp"
#include "PackDownloadProgress.hpp"
#include "SessionEntityTypes.hpp"
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
  const ConnectionPhase phase = loadContentOf(accepted);
  if (phase == ConnectionPhase::Accepted ||
      phase == ConnectionPhase::LoadingContent) {
    openSession(origin, accepted, phase);
  } else {
    leaveAtOnce(origin, phase);
  }
}

bool HandshakeAnswers::followContent() {
  ContentLoading& loading = parts_.content.get();
  const ConnectionPhase phase = phaseAfter(loading.follow());
  ConnectionStatus& status = parts_.status.get();
  if (phase == ConnectionPhase::LoadingContent) {
    const PackDownloadProgress progress = loading.progress();
    status.contentReceived = progress.received;
    return false;
  }
  status = {.phase = phase,
            .playerNumber = status.playerNumber,
            .contentSize = status.contentSize};
  if (phase == ConnectionPhase::Accepted) {
    parts_.logger.get().info("the content pack of the server is loaded");
    return false;
  }
  parts_.logger.get().warn(
      "the content pack of the server could not be loaded: it leaves the "
      "server");
  return true;
}

ConnectionPhase HandshakeAnswers::loadContentOf(
    const network::ConnectionAccepted& accepted) {
  SessionEntityTypes& types = parts_.entityTypes.get();
  if (accepted.content.fingerprint == types.localFingerprint()) {
    types.forget();
    return ConnectionPhase::Accepted;
  }
  parts_.logger.get().info(
      "the server plays with other content: pack ",
      network::fingerprintText(accepted.content.fingerprint), ", ",
      accepted.content.size, " bytes");
  return phaseAfter(parts_.content.get().begin(
      accepted.content, parts_.outbox.get().server().address));
}

ConnectionPhase HandshakeAnswers::phaseAfter(ContentStep step) const {
  switch (step) {
    case ContentStep::Ready:
      if (std::optional<PackContent> content =
              parts_.content.get().takeContent()) {
        parts_.entityTypes.get().use(std::move(*content));
      }
      return ConnectionPhase::Accepted;
    case ContentStep::Loading:
      return ConnectionPhase::LoadingContent;
    case ContentStep::DownloadFailed:
      return ConnectionPhase::ContentDownloadFailed;
    case ContentStep::Refused:
      break;
  }
  return ConnectionPhase::ContentRefused;
}

void HandshakeAnswers::openSession(const network::MessageOrigin& origin,
                                   const network::ConnectionAccepted& accepted,
                                   ConnectionPhase phase) const {
  parts_.status.get() = {.phase = phase,
                         .playerNumber = accepted.playerNumber,
                         .contentSize = accepted.content.size};
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

void HandshakeAnswers::leaveAtOnce(const network::MessageOrigin& origin,
                                   ConnectionPhase ending) const {
  parts_.outbox.get().openSession(origin.header.sessionToken);
  parts_.departure.get().leave(parts_.outbox.get());
  parts_.outbox.get().closeSession();
  parts_.status.get() = {.phase = ending};
  parts_.logger.get().warn(
      "accepted, but the content pack of the server cannot be loaded: it "
      "leaves the server");
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

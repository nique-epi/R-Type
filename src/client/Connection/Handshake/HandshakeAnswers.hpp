#pragma once

#include "ConnectionAccepted.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionPhase.hpp"
#include "ConnectionRedirect.hpp"
#include "ConnectionRefused.hpp"
#include "ContentStep.hpp"
#include "HandshakeAnswersParts.hpp"
#include "MessageOrigin.hpp"

namespace rtype::client {

/**
 * @brief What the session does with each answer of the server to an attempt.
 * Building it registers a handler per answer on the dispatcher; each answer
 * counts only while the attempt is Connecting, so an acceptance the server
 * repeats, in answer to a request that crossed the first, changes nothing.
 *
 * A ConnectionChallenge is answered at once with the request carrying its
 * cookie. A ConnectionAccepted opens the session: the outbox and the inbound
 * side take its token, and the observers learn the player. When it announces
 * the client's own content, the status is Accepted; when the cache keeps its
 * pack, the session plays with it and the status is Accepted too; otherwise
 * the pack downloads while the session holds its seat, the status is
 * LoadingContent, and followContent() moves it on. A pack that cannot be
 * downloaded or read opens nothing, or leaves the session: a Disconnection
 * carrying its token goes out at once, then twice more (see SessionDeparture),
 * so the server frees the seat it gave, and the status says why
 * (ContentDownloadFailed, ContentRefused), logged at the warning level. A
 * ConnectionRefused gives the status Refused with its reason. A
 * ConnectionRedirect, the first of the attempt, aims the session at the port it
 * names, at the address it came from, and asks again there with no cookie, as
 * for a server met for the first time; the delay of the attempt does not start
 * again, and a redirect from the game it led to is ignored.
 */
class HandshakeAnswers {
 public:
  explicit HandshakeAnswers(HandshakeAnswersParts parts);

  /**
   * @brief Follows the download of the pack while the status is
   * LoadingContent: the bytes received go to the status, and once the pack is
   * read the status is Accepted.
   * @returns Whether the session must be left: the pack could not be
   * downloaded or read, and the status says so.
   */
  [[nodiscard]] bool followContent();

 private:
  [[nodiscard]] bool isConnecting() const;
  void onChallenge(const network::ConnectionChallenge& challenge);
  void onAccepted(const network::MessageOrigin& origin,
                  const network::ConnectionAccepted& accepted);
  void onRefused(const network::MessageOrigin& origin,
                 const network::ConnectionRefused& refused);

  /** @brief Follows a redirect: asks again at the port it names, at the
   * address of the server that sent it, once per attempt. */
  void onRedirected(const network::ConnectionRedirect& redirect);

  /** @brief Opens the session @p accepted gives, its status @p phase. */
  void openSession(const network::MessageOrigin& origin,
                   const network::ConnectionAccepted& accepted,
                   ConnectionPhase phase) const;

  /** @brief Leaves at once the session the acceptance @p origin carries the
   * token of, for the content @p ending names. */
  void leaveAtOnce(const network::MessageOrigin& origin,
                   ConnectionPhase ending) const;

  /** @returns The phase of a session that plays the content @p accepted
   * announces: Accepted when it is the client's own or is read at once,
   * LoadingContent while it downloads, or why it cannot be loaded. */
  [[nodiscard]] ConnectionPhase loadContentOf(
      const network::ConnectionAccepted& accepted);

  /** @returns The phase @p step leads to; a pack read is kept as the content
   * of the session. */
  [[nodiscard]] ConnectionPhase phaseAfter(ContentStep step) const;

  HandshakeAnswersParts parts_;
};

}  // namespace rtype::client

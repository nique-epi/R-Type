#pragma once

#include <string>
#include <vector>
#include "ConnectionAccepted.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionRedirect.hpp"
#include "ConnectionRefused.hpp"
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
 * cookie. A ConnectionAccepted whose table of entity types this client draws
 * whole opens the session: the status is Accepted, the table is kept for the
 * session (see SessionEntityTypes), the outbox and the inbound side take its
 * token, and the observers learn the player. One whose table names a type this
 * client cannot draw opens nothing: a Disconnection carrying its token goes out
 * at once, then twice more (see SessionDeparture), so the server frees the seat
 * it gave; the status is MissingEntitySheet with the first such name and how
 * many others there are, and the names are logged at the warning level. A
 * ConnectionRefused gives the status Refused with its reason. A
 * ConnectionRedirect, the first of the attempt, aims the session at the port it
 * names, at the address it came from, and asks again there with no cookie, as
 * for a server met for the first time; the delay of the attempt does not start
 * again, and a redirect from the game it led to is ignored.
 */
class HandshakeAnswers {
 public:
  explicit HandshakeAnswers(HandshakeAnswersParts parts);

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

  /** @brief Opens the session @p accepted gives, keeping its table. */
  void openSession(const network::MessageOrigin& origin,
                   const network::ConnectionAccepted& accepted) const;

  /** @brief Leaves at once the session the acceptance @p origin carries the
   * token of, whose table names @p undrawable, which this client cannot draw;
   * @p undrawable holds at least one name. */
  void leaveForMissingSheets(const network::MessageOrigin& origin,
                             const std::vector<std::string>& undrawable) const;

  HandshakeAnswersParts parts_;
};

}  // namespace rtype::client

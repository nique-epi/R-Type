#pragma once

#include "ConnectionAccepted.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionRefused.hpp"
#include "HandshakeAnswersParts.hpp"
#include "MessageOrigin.hpp"

namespace rtype::client {

/**
 * @brief What the session does with each answer of the server to an attempt.
 * Building it registers a handler per answer on the dispatcher; each answer
 * counts only while the attempt is Connecting.
 *
 * A ConnectionChallenge is answered at once with the request carrying its
 * cookie. A ConnectionAccepted opens the session: the status is Accepted, the
 * outbox and the inbound side take its token, and the observers learn the
 * player. A ConnectionRefused gives the status Refused with its reason.
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

  HandshakeAnswersParts parts_;
};

}  // namespace rtype::client

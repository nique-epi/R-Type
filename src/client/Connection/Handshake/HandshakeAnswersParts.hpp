#pragma once

#include <functional>
#include "ConnectionAttempt.hpp"
#include "ConnectionStatus.hpp"
#include "Logger.hpp"
#include "MessageDispatcher.hpp"
#include "SessionDeparture.hpp"
#include "SessionEntityTypes.hpp"
#include "SessionInbound.hpp"
#include "SessionObservers.hpp"
#include "SessionOutbox.hpp"

namespace rtype::client {

/**
 * @brief What HandshakeAnswers reads and changes when the server answers an
 * attempt: the session it belongs to. Every reference must outlive the
 * answers.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct HandshakeAnswersParts {
  std::reference_wrapper<network::MessageDispatcher> dispatcher;
  std::reference_wrapper<ConnectionStatus> status;
  std::reference_wrapper<ConnectionAttempt> attempt;
  std::reference_wrapper<SessionOutbox> outbox;
  std::reference_wrapper<SessionInbound> inbound;
  std::reference_wrapper<SessionObservers> observers;
  /** @brief Sends the Disconnection of a session left at once, for a table
   * this client cannot draw. */
  std::reference_wrapper<SessionDeparture> departure;
  /** @brief Judges the table of an acceptance, and keeps it for the session
   * it opens. */
  std::reference_wrapper<SessionEntityTypes> entityTypes;
  std::reference_wrapper<logging::Logger> logger;
};

}  // namespace rtype::client

#pragma once

#include <functional>
#include <vector>
#include "IRoutedLink.hpp"
#include "ISessionObserver.hpp"
#include "MessageDispatcher.hpp"

namespace rtype::client {

/**
 * @brief What a ServerSession is built from. Every reference must outlive the
 * session.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct ServerSessionParts {
  /** @brief The datagrams of the server come in through it and the ones for
   * the server go out through it. */
  std::reference_wrapper<IRoutedLink> link;
  /** @brief Hands the messages of the server to their handlers: the session
   * registers those of the handshake, the observers the others. */
  std::reference_wrapper<network::MessageDispatcher> dispatcher;
  /** @brief The parts that follow the session, told when it opens, closes and
   * holds. */
  std::vector<std::reference_wrapper<ISessionObserver>> observers;
};

}  // namespace rtype::client

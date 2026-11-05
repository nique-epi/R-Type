#pragma once

#include <cstdint>
#include "DeltaWorldState.hpp"
#include "ServerSessionFixture.hpp"

namespace rtype::client::testing {

/** @brief The entity message queue of a session wired as the connection
 * module wires it: the tests drive the session with datagrams and drain the
 * queue. */
class EntityMessageQueueFixture : public ServerSessionFixture {
 protected:
  /**
   * @brief Delivers the first part of @p delta in a datagram of the session,
   * carrying @p sequence.
   */
  void deliverDeltaInSession(const network::DeltaWorldState& delta,
                             std::uint16_t sequence);
};

}  // namespace rtype::client::testing

#pragma once

#include "ServerSessionFixture.hpp"

namespace rtype::client::testing {

/** @brief The waiting room of a session wired as the connection module wires
 * it: the tests drive the session with datagrams and read the room. */
class WaitingRoomFixture : public ServerSessionFixture {};

}  // namespace rtype::client::testing

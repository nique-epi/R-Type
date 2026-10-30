#pragma once

#include "ServerSessionFixture.hpp"

namespace rtype::client::testing {

/** @brief The entity message queue of a session wired as the connection
 * module wires it: the tests drive the session with datagrams and drain the
 * queue. */
class EntityMessageQueueFixture : public ServerSessionFixture {};

}  // namespace rtype::client::testing

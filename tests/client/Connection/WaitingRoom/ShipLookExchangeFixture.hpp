#pragma once

#include "ServerSessionFixture.hpp"

namespace rtype::client::testing {

/** @brief The look exchange of a session wired as the connection module wires
 * it: the tests drive the session with datagrams and read the exchange. */
class ShipLookExchangeFixture : public ServerSessionFixture {};

}  // namespace rtype::client::testing

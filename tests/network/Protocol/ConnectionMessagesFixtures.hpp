#pragma once

#include "ConnectionRequest.hpp"

namespace rtype::network::testing {

/** @returns The smallest request the protocol allows: cookie 0 and a nickname
 * of one byte. */
ConnectionRequest smallestRequest();

}  // namespace rtype::network::testing

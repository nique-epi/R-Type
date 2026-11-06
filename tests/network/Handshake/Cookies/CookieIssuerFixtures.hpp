#pragma once

#include "Endpoint.hpp"

namespace rtype::network::testing {

/** @returns The sender of the cookie tests: SENDER_ADDRESS, port SENDER_PORT.
 */
[[nodiscard]] Endpoint sender();

}  // namespace rtype::network::testing

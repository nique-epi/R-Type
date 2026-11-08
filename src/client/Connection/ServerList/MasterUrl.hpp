#pragma once

#include <string>
#include "Endpoint.hpp"

namespace rtype::client {

/** @returns The url of the master at @p master: MASTER_URL_SCHEME, then its
 * address and its port as network::endpointText() writes them. */
[[nodiscard]] std::string masterUrlOf(const network::Endpoint& master);

}  // namespace rtype::client

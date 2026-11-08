#pragma once

#include <optional>
#include <string>
#include <string_view>
#include "Endpoint.hpp"

namespace rtype::client {

/** @returns The url of the master at @p master: MASTER_URL_SCHEME, then its
 * address and its port as network::endpointText() writes them. */
[[nodiscard]] std::string masterUrlOf(const network::Endpoint& master);

/** @returns The IPv4 address and the port @p url names, as masterUrlOf()
 * writes them and network::endpointFromText() reads them; nothing for a url
 * that names a host, has no port, or has anything after its port. */
[[nodiscard]] std::optional<network::Endpoint> masterEndpointOf(
    std::string_view url);

/** @returns @p url without its MASTER_URL_SCHEME, as a player reads it; @p url
 * as it is when it does not start with it. */
[[nodiscard]] std::string_view masterTextOf(std::string_view url);

}  // namespace rtype::client

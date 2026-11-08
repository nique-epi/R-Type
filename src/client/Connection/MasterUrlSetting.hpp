#pragma once

#include <string>
#include <string_view>

namespace rtype::client {

/** @returns The master the server map reads when the `master-url` setting
 * names none and the player chose none, on this machine: masterUrlOf()
 * network::LOOPBACK_IPV4_ADDRESS and network::DEFAULT_MASTER_PORT. */
[[nodiscard]] std::string defaultMasterUrl();

/**
 * @brief Refuses a url network::MasterClient cannot call: a scheme other than
 * `http://`, or a port outside 1 to 65535. As cpp-httplib reads it, a url with
 * no scheme is an `http` one, and what follows the port is never read.
 * @throws engine::CommandRefusedException naming @p url.
 */
void requireMasterUrl(std::string_view url);

}  // namespace rtype::client

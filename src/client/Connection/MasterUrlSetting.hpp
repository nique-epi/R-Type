#pragma once

#include <string>
#include <string_view>

namespace rtype::client {

/** @returns The master the server map reads when the `master-url` setting
 * names none, on this machine: MASTER_URL_SCHEME, then
 * network::LOOPBACK_IPV4_ADDRESS and network::DEFAULT_MASTER_PORT joined by
 * MASTER_PORT_SEPARATOR. */
[[nodiscard]] std::string defaultMasterUrl();

/**
 * @brief Refuses a url network::MasterClient cannot call: any scheme other
 * than `http://`. A text with no scheme reads as a host name, as cpp-httplib
 * reads it, and the server map then finds no master there.
 * @throws engine::CommandRefusedException naming @p url.
 */
void requireMasterUrl(std::string_view url);

}  // namespace rtype::client

#pragma once

#include <cstddef>
#include <string>

namespace rtype::network {

/**
 * @returns The name of the message whose type byte is @p type, as the protocol
 * page writes it, or `type-` and the number for a byte no message has.
 */
[[nodiscard]] std::string messageTypeText(std::size_t type);

}  // namespace rtype::network

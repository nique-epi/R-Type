#pragma once

#include <cstdint>
#include <string>

namespace rtype::network {

/**
 * @returns The name of the message whose type byte is @p typeByte, as the
 * protocol page writes it, or `type-` and the number for a byte no message has.
 */
[[nodiscard]] std::string messageTypeText(std::uint8_t typeByte);

}  // namespace rtype::network

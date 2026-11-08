#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "Endpoint.hpp"

namespace rtype::network {

/**
 * @returns the port written in @p text.
 * @throws InvalidPortException unless @p text holds decimal digits only and
 *         names a port from LOWEST_PORT to HIGHEST_PORT: no sign, no space,
 *         nothing after the digits.
 */
[[nodiscard]] std::uint16_t portFromText(std::string_view text);

/**
 * @returns @p text, an IPv4 address written as four decimal numbers from 0
 *          to 255 separated by dots, such as "127.0.0.1".
 *
 * The rule is the same on every system, unlike the parser Asio hands the
 * text to: `inet_pton` on Linux and macOS, `WSAStringToAddress` on Windows,
 * whose documentation says a missing port defaults to zero, so its text may
 * carry one. A number may not start with 0, unless it is 0: the macOS
 * `inet_pton` reads "01" as 1, while `inet_aton` reads a leading 0 as octal.
 *
 * @throws InvalidIpv4AddressException for any other text, a host name or an
 *         IPv6 address included.
 */
[[nodiscard]] std::string ipv4AddressFromText(std::string_view text);

/** @returns @p endpoint as `<address>:<port>`, the port in decimal digits. */
[[nodiscard]] std::string endpointText(const Endpoint& endpoint);

/**
 * @returns The endpoint @p text names as `<IPv4 address>:<port>`, both read
 *          as ipv4AddressFromText() and portFromText() read them; nothing for
 *          any other text, a host name, a missing port or anything after the
 *          port included.
 */
[[nodiscard]] std::optional<Endpoint> endpointFromText(std::string_view text);

}  // namespace rtype::network

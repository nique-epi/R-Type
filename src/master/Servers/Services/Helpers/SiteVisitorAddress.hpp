#pragma once

#include <optional>
#include <string>
#include <string_view>
#include "SiteClaim.hpp"

namespace rtype::master {

/**
 * @returns The address of the visitor @p claim names, when the master may
 * believe it; nothing otherwise, and the request is then counted under the
 * address it came from.
 *
 * The claim is believed only when @p siteKey, the `site-key` setting, is not
 * empty and the key of the claim equals it (network::secretsMatch(), in
 * constant time): a client without the secret never chooses its address, and
 * a master without the setting believes nobody, an empty key included. The
 * address must then be an IPv4 address as network::ipv4AddressFromText()
 * reads one, or look like an IPv6 one: MAXIMUM_VISITOR_ADDRESS_LENGTH
 * characters at most, all of IPV6_ADDRESS_CHARACTERS, with
 * FEWEST_IPV6_GROUP_SEPARATORS colons at least. An IPv6 address is not parsed
 * further, and is kept as written, since only the holder of the secret
 * writes it.
 */
[[nodiscard]] std::optional<std::string> visitorAddressOf(
    const SiteClaim& claim, std::string_view siteKey);

}  // namespace rtype::master

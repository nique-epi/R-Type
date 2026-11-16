#include "SiteVisitorAddress.hpp"
#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include "EndpointText.hpp"
#include "Hashing.hpp"
#include "NetworkException.hpp"
#include "SecretComparison.hpp"
#include "SiteClaim.hpp"
#include "SiteVisitorAddressConstants.hpp"

namespace rtype::master {

namespace {

bool isIpv4Address(std::string_view text) {
  try {
    static_cast<void>(network::ipv4AddressFromText(text));
    return true;
  } catch (const network::InvalidIpv4AddressException&) {
    return false;
  }
}

bool looksLikeIpv6Address(std::string_view text) {
  return text.find_first_not_of(IPV6_ADDRESS_CHARACTERS) ==
             std::string_view::npos &&
         std::ranges::count(text, IPV6_GROUP_SEPARATOR) >=
             FEWEST_IPV6_GROUP_SEPARATORS;
}

/** @returns @p text as the quota keeps an address, nothing when it is none. */
std::optional<std::string> addressIn(std::string_view text) {
  if (text.size() > MAXIMUM_VISITOR_ADDRESS_LENGTH ||
      (!isIpv4Address(text) && !looksLikeIpv6Address(text))) {
    return std::nullopt;
  }
  return std::string{text};
}

}  // namespace

std::optional<std::string> visitorAddressOf(const SiteClaim& claim,
                                            std::string_view siteKey) {
  if (siteKey.empty() || !network::secretsMatch(network::SecretComparison{
                             .presented = claim.key, .expected = siteKey})) {
    return std::nullopt;
  }
  return addressIn(claim.clientAddress);
}

}  // namespace rtype::master

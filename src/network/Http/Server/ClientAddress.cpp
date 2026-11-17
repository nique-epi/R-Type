#include "ClientAddress.hpp"
#include <cstddef>
#include <string>
#include <string_view>
#include "ForwardedAddress.hpp"
#include "HttpServerConstants.hpp"
#include "LowercaseText.hpp"
#include "NetworkConstants.hpp"
#include "PeerAddresses.hpp"

namespace rtype::network {

namespace {

/** @returns The last address of @p forwardedFor without the blanks around
 * it; empty when there is none. */
std::string_view lastForwardedAddressIn(std::string_view forwardedFor) {
  const std::size_t separator = forwardedFor.rfind(FORWARDED_ADDRESS_SEPARATOR);
  const std::string_view last = separator == std::string_view::npos
                                    ? forwardedFor
                                    : forwardedFor.substr(separator + 1);
  const std::size_t first = last.find_first_not_of(FORWARDED_ADDRESS_BLANKS);
  if (first == std::string_view::npos) {
    return {};
  }
  const std::size_t end = last.find_last_not_of(FORWARDED_ADDRESS_BLANKS);
  return last.substr(first, end - first + 1);
}

/** @returns Whether a request with @p addresses came from the proxy on the
 * machine, which a server built with @p trust believes. */
bool fromTrustedProxy(const PeerAddresses& addresses, ForwardedAddress trust) {
  return trust == ForwardedAddress::TrustedFromLoopback &&
         addresses.peer == LOOPBACK_IPV4_ADDRESS;
}

}  // namespace

std::string clientAddressOf(const PeerAddresses& addresses,
                            ForwardedAddress trust) {
  if (!fromTrustedProxy(addresses, trust)) {
    return addresses.peer;
  }
  const std::string_view forwarded =
      lastForwardedAddressIn(addresses.forwardedFor);
  return forwarded.empty() ? addresses.peer : std::string{forwarded};
}

bool reachedOverHttps(const PeerAddresses& addresses, ForwardedAddress trust) {
  return fromTrustedProxy(addresses, trust) &&
         lowercaseOf(addresses.forwardedProto) == HTTPS_SCHEME;
}

}  // namespace rtype::network

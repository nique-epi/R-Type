#include "MasterUrl.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include "Endpoint.hpp"
#include "EndpointText.hpp"
#include "NetworkException.hpp"
#include "ServerListConstants.hpp"

namespace rtype::client {

std::string masterUrlOf(const network::Endpoint& master) {
  return std::string{MASTER_URL_SCHEME} + master.address +
         std::string{MASTER_PORT_SEPARATOR} + std::to_string(master.port);
}

std::optional<network::Endpoint> masterEndpointOf(std::string_view url) {
  const std::string_view text = masterTextOf(url);
  const std::size_t separator = text.rfind(MASTER_PORT_SEPARATOR);
  if (separator == std::string_view::npos) {
    return std::nullopt;
  }
  try {
    return network::Endpoint{
        .address = network::ipv4AddressFromText(text.substr(0, separator)),
        .port = network::portFromText(text.substr(separator + 1))};
  } catch (const network::NetworkException&) {
    return std::nullopt;
  }
}

std::string_view masterTextOf(std::string_view url) {
  if (url.starts_with(MASTER_URL_SCHEME)) {
    url.remove_prefix(MASTER_URL_SCHEME.size());
  }
  return url;
}

}  // namespace rtype::client

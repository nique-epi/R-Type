#include "MasterUrl.hpp"
#include <optional>
#include <string>
#include <string_view>
#include "Endpoint.hpp"
#include "EndpointText.hpp"
#include "ServerListConstants.hpp"

namespace rtype::client {

std::string masterUrlOf(const network::Endpoint& master) {
  return std::string{MASTER_URL_SCHEME} + network::endpointText(master);
}

std::optional<network::Endpoint> masterEndpointOf(std::string_view url) {
  return network::endpointFromText(masterTextOf(url));
}

std::string_view masterTextOf(std::string_view url) {
  if (url.starts_with(MASTER_URL_SCHEME)) {
    url.remove_prefix(MASTER_URL_SCHEME.size());
  }
  return url;
}

}  // namespace rtype::client

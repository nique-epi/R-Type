#include "MasterUrl.hpp"
#include <string>
#include "Endpoint.hpp"
#include "EndpointText.hpp"
#include "ServerListConstants.hpp"

namespace rtype::client {

std::string masterUrlOf(const network::Endpoint& master) {
  return std::string{MASTER_URL_SCHEME} + network::endpointText(master);
}

}  // namespace rtype::client

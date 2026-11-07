#include "MasterUrlSetting.hpp"
#include <string>
#include <string_view>
#include "EngineException.hpp"
#include "MasterAccess.hpp"
#include "MasterClient.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "ServerListConstants.hpp"

namespace rtype::client {

std::string defaultMasterUrl() {
  return std::string{MASTER_URL_SCHEME} +
         std::string{network::LOOPBACK_IPV4_ADDRESS} +
         std::string{MASTER_PORT_SEPARATOR} +
         std::to_string(network::DEFAULT_MASTER_PORT);
}

void requireMasterUrl(std::string_view url) {
  try {
    static_cast<void>(network::MasterClient{
        network::MasterAccess{.url = std::string{url}, .key = std::string{}}});
  } catch (const network::InvalidMasterUrlException& invalid) {
    throw engine::CommandRefusedException(invalid.what());
  }
}

}  // namespace rtype::client

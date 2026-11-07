#include "MasterUrlSetting.hpp"
#include <string>
#include <string_view>
#include "Endpoint.hpp"
#include "EngineException.hpp"
#include "MasterAccess.hpp"
#include "MasterClient.hpp"
#include "MasterUrl.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"

namespace rtype::client {

std::string defaultMasterUrl() {
  return masterUrlOf(
      network::Endpoint{.address = std::string{network::LOOPBACK_IPV4_ADDRESS},
                        .port = network::DEFAULT_MASTER_PORT});
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

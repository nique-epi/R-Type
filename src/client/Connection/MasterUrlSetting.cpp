#include "MasterUrlSetting.hpp"
#include <string>
#include <string_view>
#include "ClientConstants.hpp"
#include "EngineException.hpp"
#include "MasterAccess.hpp"
#include "MasterClient.hpp"
#include "NetworkException.hpp"

namespace rtype::client {

std::string defaultMasterUrl() { return std::string{DEFAULT_MASTER_URL}; }

void requireMasterUrl(std::string_view url) {
  try {
    static_cast<void>(network::MasterClient{
        network::MasterAccess{.url = std::string{url}, .key = std::string{}}});
  } catch (const network::InvalidMasterUrlException& invalid) {
    throw engine::CommandRefusedException(invalid.what());
  }
}

}  // namespace rtype::client

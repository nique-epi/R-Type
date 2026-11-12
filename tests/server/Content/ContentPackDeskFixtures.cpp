#include "ContentPackDeskFixtures.hpp"
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include "ApiRequest.hpp"
#include "ContentPack.hpp"
#include "ContentPackFixtures.hpp"
#include "ContentRouteConstants.hpp"

namespace rtype::server::testing {

std::vector<std::byte> somePack() {
  return network::encodeContentPack(network::testing::twoContentFiles());
}

network::ApiRequest packRequest(std::string_view address,
                                std::string_view fingerprint) {
  return network::ApiRequest{
      .body = {},
      .remoteAddress = std::string{address},
      .headers = {},
      .pathParameters = {{std::string{network::FINGERPRINT_PATH_PARAMETER},
                          std::string{fingerprint}}},
      .queryParameters = {}};
}

}  // namespace rtype::server::testing

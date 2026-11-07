#include "ContentPackDeskFixtures.hpp"
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include "ApiRequest.hpp"
#include "ContentFile.hpp"
#include "ContentPack.hpp"
#include "ContentPackTestConstants.hpp"
#include "ContentRouteConstants.hpp"

namespace rtype::server::testing {

std::vector<std::byte> somePack() {
  const std::vector<identifiers::ContentFile> files{
      identifiers::ContentFile{
          .path = std::string{network::testing::DRIFTER_SHEET_PATH},
          .bytes = std::string{network::testing::DRIFTER_SHEET_BYTES}},
      identifiers::ContentFile{
          .path = std::string{network::testing::BYDOMANCER_SPRITES_PATH},
          .bytes = std::string{network::testing::BYDOMANCER_PIXELS}}};
  return network::encodeContentPack(files);
}

std::string textOf(const std::vector<std::byte>& pack) {
  std::string text;
  for (const std::byte value : pack) {
    text.push_back(static_cast<char>(value));
  }
  return text;
}

network::ApiRequest packRequest(std::string_view address,
                                std::string_view fingerprint) {
  return network::ApiRequest{
      .body = {},
      .remoteAddress = std::string{address},
      .pathParameters = {{std::string{network::FINGERPRINT_PATH_PARAMETER},
                          std::string{fingerprint}}},
      .queryParameters = {}};
}

}  // namespace rtype::server::testing

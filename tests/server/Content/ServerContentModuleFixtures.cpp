#include "ServerContentModuleFixtures.hpp"
#include <httplib.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>
#include "AssetFolder.hpp"
#include "ContentFile.hpp"
#include "ContentFiles.hpp"
#include "ContentPack.hpp"
#include "ContentPackRoute.hpp"
#include "NetworkConstants.hpp"
#include "ServerContentModuleTestConstants.hpp"

std::string pathOfTheLocalPack() {
  const std::array<std::filesystem::path, 1> contentFolders{
      rtype::engine::locateAssetFolder()};
  const std::vector<rtype::identifiers::ContentFile> files =
      rtype::game::contentFilesIn(contentFolders);
  return rtype::network::contentPackPathOf(rtype::network::contentFingerprintOf(
      rtype::network::encodeContentPack(files)));
}

// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
httplib::Result fetchedFrom(std::uint16_t port, const std::string& path) {
  httplib::Client client{std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
                         port};
  return client.Get(path);
}

httplib::Result fetchedFromOffset(std::uint16_t port, const std::string& path,
                                  std::size_t offset) {
  httplib::Client client{std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
                         port};
  const httplib::Headers headers{httplib::make_range_header(
      {httplib::Range{static_cast<httplib::Range::first_type>(offset),
                      rtype::server::testing::NO_RANGE_END}})};
  return client.Get(path, headers);
}
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)

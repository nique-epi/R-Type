#include "ContentPackDownloadFixtures.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <thread>
#include <vector>
#include "ContentPackDownload.hpp"
#include "ContentPackDownloadTestConstants.hpp"
#include "ContentPackTestFiles.hpp"
#include "NetworkConstants.hpp"
#include "PackDownloadOutcome.hpp"
#include "PackDownloadProgress.hpp"
#include "PackDownloadTarget.hpp"

namespace rtype::client::testing {

PackDownloadProgress endOf(const ContentPackDownload& download) {
  const auto deadline = std::chrono::steady_clock::now() + DOWNLOAD_WAIT;
  PackDownloadProgress progress = download.progress();
  while (progress.outcome == PackDownloadOutcome::Running &&
         std::chrono::steady_clock::now() < deadline) {
    std::this_thread::sleep_for(DOWNLOAD_POLL);
    progress = download.progress();
  }
  return progress;
}

std::string lancerBody() { return textOf(lancerPack()); }

PackDownloadTarget packAt(std::uint16_t port, std::size_t size) {
  return PackDownloadTarget{.host = std::string{network::LOOPBACK_IPV4_ADDRESS},
                            .port = port,
                            .path = std::string{PACK_PATH},
                            .size = size};
}

std::string textOf(const std::vector<std::byte>& bytes) {
  std::string text;
  for (const std::byte value : bytes) {
    text.push_back(static_cast<char>(value));
  }
  return text;
}

}  // namespace rtype::client::testing

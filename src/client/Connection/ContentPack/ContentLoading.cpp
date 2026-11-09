#include "ContentLoading.hpp"
#include <cstddef>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "ContentAnnouncement.hpp"
#include "ContentFile.hpp"
#include "ContentPack.hpp"
#include "ContentPackCache.hpp"
#include "ContentPackDownload.hpp"
#include "ContentPackRoute.hpp"
#include "ContentStep.hpp"
#include "NetworkConstants.hpp"
#include "PackContent.hpp"
#include "PackContentReading.hpp"
#include "PackDownloadConstants.hpp"
#include "PackDownloadOutcome.hpp"
#include "PackDownloadProgress.hpp"
#include "PackDownloadTarget.hpp"

namespace rtype::client {

ContentLoading::ContentLoading(ContentPackCache cache)
    : cache_(std::move(cache)) {}

ContentStep ContentLoading::begin(const network::ContentAnnouncement& announced,
                                  const std::string& host) {
  cancel();
  announced_ = announced;
  const std::optional<std::vector<identifiers::ContentFile>> kept =
      cache_.find(announced.fingerprint);
  if (kept.has_value()) {
    return read(*kept);
  }
  if (announced.port == network::ANY_PORT) {
    return ContentStep::DownloadFailed;
  }
  download_ = std::make_unique<ContentPackDownload>(PackDownloadTarget{
      .origin = std::string{HTTP_ORIGIN_PREFIX} + host + ':' +
                std::to_string(announced.port),
      .path = network::contentPackPathOf(announced.fingerprint),
      .size = announced.size,
      .followsRedirects = false});
  return ContentStep::Loading;
}

ContentStep ContentLoading::follow() {
  if (download_ == nullptr) {
    return ContentStep::DownloadFailed;
  }
  const PackDownloadOutcome outcome = download_->progress().outcome;
  if (outcome == PackDownloadOutcome::Running) {
    return ContentStep::Loading;
  }
  const std::vector<std::byte> pack = download_->takeBytes();
  download_.reset();
  if (outcome == PackDownloadOutcome::Failed) {
    return ContentStep::DownloadFailed;
  }
  return readDownloaded(pack);
}

PackDownloadProgress ContentLoading::progress() const {
  if (download_ == nullptr) {
    return PackDownloadProgress{.received = 0,
                                .outcome = PackDownloadOutcome::Running};
  }
  return download_->progress();
}

std::optional<PackContent> ContentLoading::takeContent() {
  return std::exchange(content_, std::nullopt);
}

void ContentLoading::cancel() {
  download_.reset();
  content_.reset();
}

ContentStep ContentLoading::readDownloaded(std::span<const std::byte> pack) {
  const std::optional<std::vector<identifiers::ContentFile>> files =
      network::filesOfPack(pack, announced_.fingerprint);
  if (!files.has_value()) {
    return ContentStep::Refused;
  }
  cache_.keep(announced_.fingerprint, pack);
  return read(*files);
}

ContentStep ContentLoading::read(
    std::span<const identifiers::ContentFile> files) {
  try {
    content_ =
        packContentOf(files, cache_.spriteFolderOf(announced_.fingerprint));
  } catch (const std::runtime_error&) {
    return ContentStep::Refused;
  }
  return ContentStep::Ready;
}

}  // namespace rtype::client

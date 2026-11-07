#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include "ContentAnnouncement.hpp"
#include "ContentFile.hpp"
#include "ContentPackCache.hpp"
#include "ContentPackDownload.hpp"
#include "ContentStep.hpp"
#include "PackContent.hpp"
#include "PackDownloadProgress.hpp"

namespace rtype::client {

/**
 * @brief Loads the content a server announced when it differs from the
 * client's own: from the cache when a pack of its fingerprint is kept, else
 * by downloading it from the address the player joined.
 *
 * A pack downloaded is loaded only when it has the announced size and
 * fingerprint, then kept in the cache. A pack whose sheets this client cannot
 * read is refused, and never kept as the content of a session. Use it from
 * the frame loop only; the download runs on a thread of its own.
 */
class ContentLoading {
 public:
  explicit ContentLoading(ContentPackCache cache);

  /**
   * @brief Starts loading @p announced, served at @p host.
   * @returns Ready when the cache holds it, Loading when a download started,
   * DownloadFailed when its supervisor serves none, Refused when the kept
   * pack cannot be read.
   */
  [[nodiscard]] ContentStep begin(const network::ContentAnnouncement& announced,
                                  const std::string& host);

  /** @returns Where the download started by begin() stands: Loading while
   * bytes come, then once Ready, DownloadFailed or Refused. */
  [[nodiscard]] ContentStep follow();

  /** @returns How far the download went; nothing received when none runs. */
  [[nodiscard]] PackDownloadProgress progress() const;

  /** @returns The content read, once a step was Ready; then nothing. */
  [[nodiscard]] std::optional<PackContent> takeContent();

  /** @brief Stops a download in progress; the session that asked for it is
   * gone. */
  void cancel();

 private:
  [[nodiscard]] ContentStep readDownloaded(std::span<const std::byte> pack);
  [[nodiscard]] ContentStep read(
      std::span<const identifiers::ContentFile> files);

  ContentPackCache cache_;
  network::ContentAnnouncement announced_;
  std::unique_ptr<ContentPackDownload> download_;
  std::optional<PackContent> content_;
};

}  // namespace rtype::client

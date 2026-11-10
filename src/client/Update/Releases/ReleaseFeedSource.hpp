#pragma once

#include <Velopack.hpp>
#include <memory>
#include <string>
#include "IReleaseFiles.hpp"
#include "Logger.hpp"

namespace rtype::client {

/**
 * @brief Where Velopack reads the releases of the game: the feed of the
 * latest release, then the packages that feed lists, from the files of the
 * release themselves (`releases/latest/download/`), which GitHub serves with
 * no limit of requests, unlike its API.
 *
 * Velopack checks each package against the SHA-256 of the feed; the feed
 * itself is trusted as GitHub serves it, over HTTPS.
 *
 * It derives from Velopack::IUpdateSource, an abstract class of the library
 * that registers its callbacks in its constructor: the only way the library
 * offers to give it a source. Velopack calls it from Rust, so no exception
 * leaves it: a feed not read is an empty text, which Velopack refuses, and a
 * package not saved is false.
 */
class ReleaseFeedSource final : public Velopack::IUpdateSource {
 public:
  /** @param files Where the files are read; the source owns it. */
  explicit ReleaseFeedSource(std::unique_ptr<IReleaseFiles> files);

  /** @returns The feed of the latest release named @p releasesName, or an
   * empty text when it could not be read or its name is not a file name. */
  const std::string GetReleaseFeed(std::string releasesName) override;

  /** @brief Saves the package @p asset to @p localFilePath, from the release
   * of its version, reporting the share received to @p progress.
   * @returns Whether it was saved whole. */
  bool DownloadReleaseEntry(const Velopack::VelopackAsset& asset,
                            std::string localFilePath,
                            Velopack::vpkc_progress_send_t progress) override;

  /** @brief Ends the requests in progress, and every later one. */
  void stop();

 private:
  [[nodiscard]] std::string latestFeed(const std::string& releasesName);
  [[nodiscard]] bool saved(const Velopack::VelopackAsset& asset,
                           const std::string& localFilePath,
                           const Velopack::vpkc_progress_send_t& progress);

  std::unique_ptr<IReleaseFiles> files_;
  logging::Logger logger_;
};

}  // namespace rtype::client

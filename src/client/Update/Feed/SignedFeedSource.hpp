#pragma once

#include <Velopack.hpp>
#include <memory>
#include <span>
#include <string>
#include "FeedConstants.hpp"
#include "IReleaseFiles.hpp"
#include "Logger.hpp"

namespace rtype::client {

/**
 * @brief Where Velopack reads the releases of the game: the feed of the
 * latest release, handed over only once its Ed25519 signature is the one of
 * the key of the game, then the packages that feed lists.
 *
 * Velopack checks the SHA-256 of a package against the feed, and nothing
 * signs the feed itself: whoever could change a release could change both.
 * Checking the signature here makes the feed, and through it every package,
 * the one the release workflow signed.
 *
 * It derives from Velopack::IUpdateSource, an abstract class of the library
 * that registers its callbacks in its constructor: the only way the library
 * offers to give it a source. Velopack calls it from Rust, so no exception
 * leaves it: a feed refused is an empty text, which Velopack refuses in turn,
 * and a package not saved is false.
 */
class SignedFeedSource final : public Velopack::IUpdateSource {
 public:
  /** @param files Where the files are read; the source owns it.
   * @param publicKey Public half of the key that signs the feeds. */
  SignedFeedSource(
      std::unique_ptr<IReleaseFiles> files,
      std::span<const unsigned char, ED25519_PUBLIC_KEY_BYTES> publicKey);

  /** @returns The feed of the latest release named @p releasesName, or an
   * empty text when it, or its signature, could not be read, or when the
   * signature is not the one of the key. */
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
  [[nodiscard]] std::string trustedFeed(const std::string& releasesName);
  [[nodiscard]] bool saved(const Velopack::VelopackAsset& asset,
                           const std::string& localFilePath,
                           const Velopack::vpkc_progress_send_t& progress);

  std::unique_ptr<IReleaseFiles> files_;
  std::span<const unsigned char, ED25519_PUBLIC_KEY_BYTES> publicKey_;
  logging::Logger logger_;
};

}  // namespace rtype::client

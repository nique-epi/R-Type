#pragma once

#include <cstddef>
#include <filesystem>
#include <optional>
#include <span>
#include <vector>
#include "ContentFile.hpp"
#include "ContentFingerprint.hpp"

namespace rtype::client {

/**
 * @brief The content packs this client keeps, each in a file of a folder
 * named after its fingerprint, so a server met again needs no download.
 *
 * A pack found is checked again, size and fingerprint, every time it is
 * read: a file changed or cut on the disk is removed and never loaded. Once
 * more than MAXIMUM_CACHED_PACKS are kept, the one read or kept longest ago
 * goes, with the folder its sprite sheets were written to. The disk may
 * refuse to write: the pack is then simply not kept.
 */
class ContentPackCache {
 public:
  explicit ContentPackCache(std::filesystem::path folder);

  /** @returns The files of the pack @p fingerprint, or nothing when it is not
   * kept or no longer passes its checks. */
  [[nodiscard]] std::optional<std::vector<identifiers::ContentFile>> find(
      const network::ContentFingerprint& fingerprint) const;

  /** @brief Keeps @p pack under @p fingerprint, which the caller checked. */
  void keep(const network::ContentFingerprint& fingerprint,
            std::span<const std::byte> pack) const;

  /** @returns The folder the sprite sheets of the pack @p fingerprint are
   * written to, below the folder of the cache. */
  [[nodiscard]] std::filesystem::path spriteFolderOf(
      const network::ContentFingerprint& fingerprint) const;

 private:
  [[nodiscard]] std::filesystem::path fileOf(
      const network::ContentFingerprint& fingerprint) const;

  /** @brief Removes the packs used longest ago, past MAXIMUM_CACHED_PACKS. */
  void forgetTheOldest() const;

  std::filesystem::path folder_;
};

}  // namespace rtype::client

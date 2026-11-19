#pragma once

#include <functional>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include "AssetLibrary.hpp"
#include "IMapPreviews.hpp"
#include "ISessionPack.hpp"

namespace rtype::client {

/**
 * @brief Loads the preview of a map from the files the pack of the session
 * wrote, into the asset library, named by its whole path as the sprite sheets
 * of a pack are. A file that failed to load is not read again, so a preview
 * the pack lacks costs no read of the disk at each frame.
 */
class MapPreviews final : public IMapPreviews {
 public:
  /** @param assets Loads the texture; @param files Finds the file of the
   * pack; both must outlive the previews. */
  MapPreviews(AssetLibrary& assets, const ISessionPack& files);

  [[nodiscard]] std::optional<std::string> previewOf(
      std::string_view path) override;

 private:
  AssetLibrary* assets_;
  const ISessionPack* files_;
  std::set<std::string, std::less<>> unloadable_;
};

}  // namespace rtype::client

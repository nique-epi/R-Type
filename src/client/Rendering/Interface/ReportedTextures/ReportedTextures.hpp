#pragma once

#include <functional>
#include <set>
#include <string>
#include <string_view>
#include "AssetTextureSource.hpp"
#include "Logger.hpp"

namespace sf {
class Texture;
}  // namespace sf

namespace rtype::client {

class AssetLibrary;

/**
 * @brief Finds the textures the interface draws, and tells the log once about
 * each id it finds no texture for.
 *
 * The library must outlive it.
 */
class ReportedTextures {
 public:
  explicit ReportedTextures(const AssetLibrary& library);

  /** @returns The loaded texture of @p assetId, or nullptr once the log says
   * it is missing; a missing id is reported once. */
  [[nodiscard]] const sf::Texture* found(std::string_view assetId);

 private:
  AssetTextureSource textures_;
  logging::Logger logger_;
  std::set<std::string, std::less<>> reportedMissingIds_;
};

}  // namespace rtype::client

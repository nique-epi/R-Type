#pragma once

#include <string_view>
#include "ITextureSource.hpp"

namespace sf {
class Texture;
}  // namespace sf

namespace rtype::client {

class AssetLibrary;

/**
 * @brief Finds textures among the assets an asset library has loaded.
 *
 * It never loads anything: a texture is found once a loading step has loaded
 * it, and no longer once the library has released it. The library must
 * outlive it.
 */
class AssetTextureSource : public ITextureSource {
 public:
  explicit AssetTextureSource(const AssetLibrary& library);

  [[nodiscard]] const sf::Texture* texture(
      std::string_view assetId) const override;

 private:
  const AssetLibrary* library_;
};

}  // namespace rtype::client

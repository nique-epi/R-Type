#include "AssetTextureSource.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <string_view>
#include "AssetLibrary.hpp"

namespace rtype::client {

AssetTextureSource::AssetTextureSource(const AssetLibrary& library)
    : library_(&library) {}

const sf::Texture* AssetTextureSource::texture(std::string_view assetId) const {
  return library_->findTexture(assetId);
}

}  // namespace rtype::client

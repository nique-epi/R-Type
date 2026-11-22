#include "ReportedTextures.hpp"
#include <string>
#include <string_view>
#include "InterfaceConstants.hpp"

namespace rtype::client {

ReportedTextures::ReportedTextures(const AssetLibrary& library)
    : textures_(library), logger_(std::string(INTERFACE_LOGGER_NAME)) {}

const sf::Texture* ReportedTextures::found(std::string_view assetId) {
  const sf::Texture* texture = textures_.texture(assetId);
  if (texture == nullptr && !reportedMissingIds_.contains(assetId)) {
    reportedMissingIds_.emplace(assetId);
    logger_.warn("no texture for asset id ", assetId);
  }
  return texture;
}

}  // namespace rtype::client

#include "ReportingTextureSource.hpp"
#include <string>
#include <string_view>
#include "ITextureSource.hpp"
#include "RenderingConstants.hpp"

namespace rtype::client {

ReportingTextureSource::ReportingTextureSource(const ITextureSource& textures,
                                               std::string_view loggerName)
    : textures_(&textures), logger_(std::string(loggerName)) {}

const sf::Texture* ReportingTextureSource::found(std::string_view assetId) {
  const sf::Texture* texture = textures_->texture(assetId);
  if (texture == nullptr && reportedMissingIds_.emplace(assetId).second) {
    logger_.warn(MISSING_TEXTURE_WARNING, assetId);
  }
  return texture;
}

}  // namespace rtype::client

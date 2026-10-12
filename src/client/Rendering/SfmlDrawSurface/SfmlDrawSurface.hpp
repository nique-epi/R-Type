#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <set>
#include <string>
#include <string_view>
#include "IDrawSurface.hpp"
#include "ITextureSource.hpp"
#include "Logger.hpp"
#include "Position.hpp"

namespace rtype::client {

/**
 * @brief Draws images on an SFML render target.
 *
 * The target and the texture source must outlive it. An id the source does not
 * know is not drawn and is reported once in the log.
 */
class SfmlDrawSurface : public IDrawSurface {
 public:
  SfmlDrawSurface(sf::RenderTarget& target, const ITextureSource& textures);

  void draw(std::string_view assetId, game::Position center) override;

 private:
  sf::RenderTarget& target_;
  const ITextureSource& textures_;
  logging::Logger logger_;
  std::set<std::string, std::less<>> reportedMissingIds_;
};

}  // namespace rtype::client

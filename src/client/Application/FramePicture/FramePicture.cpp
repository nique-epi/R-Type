#include "FramePicture.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include "ClientException.hpp"
#include "PlayfieldConstants.hpp"
#include "ScreenStack.hpp"
#include "ScrollingBackground.hpp"

namespace rtype::client {

void drawFrame(sf::RenderTarget& target, ScrollingBackground& sky,
               const ScreenStack& screens) {
  sky.draw(target);
  screens.draw(target);
}

sf::Texture framePictureOf(ScrollingBackground& sky,
                           const ScreenStack& screens) {
  const sf::Vector2u size(static_cast<unsigned int>(game::PLAYFIELD_WIDTH),
                          static_cast<unsigned int>(game::PLAYFIELD_HEIGHT));
  sf::RenderTexture frame;
  if (!frame.resize(size)) {
    throw RenderTextureNotCreatedException(size.x, size.y);
  }
  frame.clear();
  drawFrame(frame, sky, screens);
  frame.display();
  return frame.getTexture();
}

}  // namespace rtype::client

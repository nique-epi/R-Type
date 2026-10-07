#include "PlayfieldViewport.hpp"
#include "PlayfieldConstants.hpp"
#include "WindowConstants.hpp"

namespace rtype::client {

PlayfieldViewport fitPlayfieldInWindow(unsigned int windowWidth,
                                       unsigned int windowHeight) {
  if (windowWidth == 0 || windowHeight == 0) {
    return {.left = 0.0F,
            .top = 0.0F,
            .width = WHOLE_WINDOW,
            .height = WHOLE_WINDOW};
  }
  const float windowRatio =
      static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
  const float playfieldRatio = game::PLAYFIELD_WIDTH / game::PLAYFIELD_HEIGHT;
  if (windowRatio > playfieldRatio) {
    const float width = playfieldRatio / windowRatio;
    return {.left = (WHOLE_WINDOW - width) * BAR_SHARE_PER_SIDE,
            .top = 0.0F,
            .width = width,
            .height = WHOLE_WINDOW};
  }
  const float height = windowRatio / playfieldRatio;
  return {.left = 0.0F,
          .top = (WHOLE_WINDOW - height) * BAR_SHARE_PER_SIDE,
          .width = WHOLE_WINDOW,
          .height = height};
}

}  // namespace rtype::client

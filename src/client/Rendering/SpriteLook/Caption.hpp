#pragma once

#include <string>
#include "BackgroundConstants.hpp"
#include "InterfaceConstants.hpp"
#include "Layer.hpp"
#include "Position.hpp"
#include "RenderingConstants.hpp"
#include "SpriteFrame.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

/**
 * @brief Component of an entity drawn as a line of text, instead of a sprite
 * (the figure of a gain) or beside it (the name of a pilot over its ship).
 * RenderSystem writes it centered on the Position of the entity moved by
 * `offset`, in its layer, while it is visible.
 */
struct Caption {
  /** @brief UTF-8 text. */
  std::string text;
  TextStyle style;
  game::Layer layer{};
  bool visible = true;
  /** @brief How far from the Position of the entity the line is centered, in
   * logical units; (0, 0) centers it on the entity. */
  game::Position offset{};
};

/**
 * @returns How far above the center of a sprite drawn with @p frame a caption
 * is centered so its line clears the top of the sprite by @p gap
 * pixels, in logical units: half the sprite and half a line, since a caption
 * is centered on its place, plus the gap.
 */
constexpr float captionRiseOver(game::SpriteFrame frame, int gap) {
  return ((static_cast<float>(frame.height + TEXT_LINE_HEIGHT) * HALF) +
          static_cast<float>(gap)) *
         BACKGROUND_PIXEL_SIZE;
}

}  // namespace rtype::client

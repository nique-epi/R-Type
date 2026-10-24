#pragma once

#include <string>
#include "Layer.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

/**
 * @brief Component of an entity drawn as a line of text instead of a sprite:
 * the figure of a gain. RenderSystem writes it centered on the Position of the
 * entity, in its layer, while it is visible.
 */
struct Caption {
  /** @brief UTF-8 text. */
  std::string text;
  TextStyle style;
  game::Layer layer{};
  bool visible = true;
};

}  // namespace rtype::client

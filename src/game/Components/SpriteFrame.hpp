#pragma once

namespace rtype::game {

/**
 * @brief The rectangle of a texture an entity shows, in pixels of the texture.
 *
 * The origin is the top-left corner of the texture, x grows to the right and y
 * grows downwards. It lets many entities share one sprite sheet.
 */
struct SpriteFrame {
  int left;
  int top;
  int width;
  int height;

  bool operator==(const SpriteFrame&) const = default;
};

}  // namespace rtype::game

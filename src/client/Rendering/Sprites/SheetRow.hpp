#pragma once

#include <array>
#include <cstddef>
#include "SpriteFrame.hpp"

namespace rtype::client {

/** @returns The first row of pixels under a frame of a sheet. */
constexpr int bottomOf(game::SpriteFrame frame) {
  return frame.top + frame.height;
}

/**
 * @returns The frame of the same size and at the same left edge as
 * `above`, `spacing` pixels under it: the first frame of the next row of
 * a sheet whose two rows hold frames of one size.
 */
constexpr game::SpriteFrame frameUnder(game::SpriteFrame above, int spacing) {
  return game::SpriteFrame{.left = above.left,
                           .top = bottomOf(above) + spacing,
                           .width = above.width,
                           .height = above.height};
}

/**
 * @returns The Count frames of a row of a sheet: the first one given, then
 * frames of the same size, each `spacing` pixels to the right of the previous
 * one, as the entity and effect sheets lay them out.
 */
template <std::size_t Count>
constexpr std::array<game::SpriteFrame, Count> framesInRow(
    game::SpriteFrame first, int spacing) {
  static_assert(Count > 0, "a row of a sheet holds at least one frame");
  std::array<game::SpriteFrame, Count> frames{};
  for (std::size_t i = 0; i < Count; ++i) {
    frames[i] = game::SpriteFrame{
        .left = first.left + (static_cast<int>(i) * (first.width + spacing)),
        .top = first.top,
        .width = first.width,
        .height = first.height};
  }
  return frames;
}

}  // namespace rtype::client

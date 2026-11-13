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
 * @returns The frame `index` places to the right of `first` in a row of a
 * sheet: same size, each `spacing` pixels to the right of the previous one,
 * as the entity and effect sheets lay them out; `first` itself for index 0.
 */
constexpr game::SpriteFrame frameInRow(game::SpriteFrame first,
                                       std::size_t index, int spacing) {
  return game::SpriteFrame{
      .left = first.left + (static_cast<int>(index) * (first.width + spacing)),
      .top = first.top,
      .width = first.width,
      .height = first.height};
}

/**
 * @returns The Count frames of a row of a sheet: the first one given, then
 * the frames frameInRow() places after it.
 */
template <std::size_t Count>
constexpr std::array<game::SpriteFrame, Count> framesInRow(
    game::SpriteFrame first, int spacing) {
  static_assert(Count > 0, "a row of a sheet holds at least one frame");
  std::array<game::SpriteFrame, Count> frames{};
  for (std::size_t i = 0; i < Count; ++i) {
    frames[i] = frameInRow(first, i, spacing);
  }
  return frames;
}

/**
 * @returns `Rows` rows of `Count` frames each: the first row starts at
 * `first`, each next one frameUnder() the row before it, as a sheet whose
 * rows all hold frames of one size lays them out.
 */
template <std::size_t Rows, std::size_t Count>
constexpr std::array<std::array<game::SpriteFrame, Count>, Rows> rowsOfFrames(
    game::SpriteFrame first, int spacing) {
  std::array<std::array<game::SpriteFrame, Count>, Rows> rows{};
  game::SpriteFrame start = first;
  for (std::size_t row = 0; row < Rows; ++row) {
    rows[row] = framesInRow<Count>(start, spacing);
    start = frameUnder(start, spacing);
  }
  return rows;
}

}  // namespace rtype::client

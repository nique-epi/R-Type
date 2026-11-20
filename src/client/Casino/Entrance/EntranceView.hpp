#pragma once

#include "ButlerLine.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

/** @brief What the front of the casino shows under its lights and its bars:
 * the picture of the front, the butler and what he says. */
struct EntranceView {
  /** @brief The picture of the front, its doors closed or open. */
  const char* front{};
  /** @brief The frame of the butler on his sheet. */
  game::SpriteFrame butler{};
  ButlerLine line;
};

}  // namespace rtype::client

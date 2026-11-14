#pragma once

#include "ScreenId.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

/** @brief A card of the destination screen: a place PLAY may lead to. */
struct DestinationCard {
  /** @brief Its name, written under its sprite. */
  const char* label{};
  /** @brief The line written under the cards while it is in the middle. */
  const char* description{};
  /** @brief The sheet of its sprite, and the sprite on it. */
  const char* sheet{};
  game::SpriteFrame frame{};
  /** @brief The screen it pushes over the destination screen. */
  ScreenId screen;
};

}  // namespace rtype::client

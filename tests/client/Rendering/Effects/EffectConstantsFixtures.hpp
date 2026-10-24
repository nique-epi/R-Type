#pragma once

#include <SFML/Graphics/Image.hpp>
#include <vector>
#include "EffectAnimations.hpp"
#include "SpriteFrame.hpp"

/** @brief What the version without flash of an effect changes: the most
 * pixels one frame changes, and how many frames, from the first, change
 * any. */
struct Change {
  int pixels{};
  int frames{};

  bool operator==(const Change&) const = default;
};

/** @returns What the version without flash of @p effect changes in
 * @p sheet, frame by frame against its standard version. */
Change changeOf(const sf::Image& sheet,
                const rtype::client::EffectAnimations& effect);

/** @returns Every frame of both versions of every effect. */
std::vector<rtype::game::SpriteFrame> everyEffectFrame();

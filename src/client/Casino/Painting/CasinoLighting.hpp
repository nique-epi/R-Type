#pragma once

#include <SFML/Graphics/Color.hpp>
#include <cstdint>
#include <vector>
#include "ColoredPixel.hpp"
#include "LightBeam.hpp"
#include "TimeConstants.hpp"
#include "TwinkleGroup.hpp"

namespace rtype::client {

/** @returns Whether @p color is a lit bulb as the pictures of the casino paint
 * one: pale cream or yellow, above the LIT_BULB_*_FLOOR of each channel. */
[[nodiscard]] bool isLitBulb(sf::Color color);

/** @returns How bright @p color looks, from 0 to 255 (ITU-R BT.601). */
[[nodiscard]] float luminanceOf(sf::Color color);

/** @returns The step of the chase of the bulbs @p since they began:
 * one more every CASINO_CHASE_STEP. */
[[nodiscard]] int chaseStepAt(engine::Duration since);

/** @returns Whether @p group is dim @p since the room opened: the sine of its
 * beat above its threshold. */
[[nodiscard]] bool isDimmedAt(const TwinkleGroup& group,
                              engine::Duration since);

/** @returns @p value scrambled, every bit of it moving every bit of the
 * result: a fixed choice that looks random, such as where a camera flashes in
 * a window of time, the same on every run. */
[[nodiscard]] std::uint32_t scrambledOf(std::uint32_t value);

/** @brief Adds to @p pixels every pixel of the interface whose middle lies
 * inside @p beam, in the color of the beam; none outside INTERFACE_AREA. */
void addBeamPixels(std::vector<ColoredPixel>& pixels, const LightBeam& beam);

}  // namespace rtype::client

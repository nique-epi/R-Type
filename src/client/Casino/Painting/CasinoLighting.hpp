#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <span>
#include <vector>
#include "BeamPair.hpp"
#include "ColoredPixel.hpp"
#include "LightBeam.hpp"
#include "TimeConstants.hpp"
#include "TwinkleBeats.hpp"
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

/** @returns CASINO_TWINKLE_GROUPS beats with no point of light yet, each
 * CASINO_BEAT_SPREAD radians after the one before, their speeds spread over
 * the range of @p beats. */
[[nodiscard]] std::vector<TwinkleGroup> twinkleBeats(const TwinkleBeats& beats);

/** @returns @p color with the alpha of @p share, from 0, clear, to 1, whole. */
[[nodiscard]] sf::Color withShareOfLight(sf::Color color, float share);

/** @returns The color of the pixel of @p picture at @p pixel, which must lie
 * inside it. */
[[nodiscard]] sf::Color pixelColorAt(const sf::Image& picture,
                                     sf::Vector2i pixel);

/** @returns Whether @p pixel lies in one of @p areas. */
[[nodiscard]] bool liesInAny(sf::Vector2i pixel,
                             std::span<const sf::IntRect> areas);

/** @returns @p pixel scrambled, both its coordinates moving every bit: the
 * fixed draw of a point of light, such as the beat it twinkles on. */
[[nodiscard]] std::uint32_t scrambledAt(sf::Vector2i pixel);

/** @returns @p value scrambled, every bit of it moving every bit of the
 * result: a fixed choice that looks random, such as where a camera flashes in
 * a window of time, the same on every run. */
[[nodiscard]] std::uint32_t scrambledOf(std::uint32_t value);

/** @brief Adds to @p pixels every pixel of the interface whose middle lies
 * inside @p beam, in the color of the beam; none outside INTERFACE_AREA. */
void addBeamPixels(std::vector<ColoredPixel>& pixels, const LightBeam& beam);

/** @brief Empties @p pixels, then adds to them the pixels of both beams of
 * @p pair as they swing @p since the room opened. */
void swingBeams(std::vector<ColoredPixel>& pixels, const BeamPair& pair,
                engine::Duration since);

}  // namespace rtype::client

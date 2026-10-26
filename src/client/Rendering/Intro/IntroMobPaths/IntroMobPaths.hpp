#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include <optional>
#include "IntroCastConstants.hpp"
#include "IntroMob.hpp"

namespace rtype::client {

/** @returns Where the center of Bydo @p index of the helix is: two strands
 * that sweep in from the right, out of phase. */
[[nodiscard]] sf::Vector2f helixCenterAt(std::size_t index, float scene);

/** @returns Where the center of newcomer @p index is: gliding in from past
 * the right edge to its place in the ring, which turns faster and faster and
 * shrinks during the charge. */
[[nodiscard]] sf::Vector2f newcomerCenterAt(std::size_t index, float scene);

/** @returns Where the center of the prey of the third close-up is. */
[[nodiscard]] sf::Vector2f preyCenterAt(float scene);

/** @returns The moment of the scene the missile meant for Bydo @p index of
 * the helix hits it: INTRO_KILL_GAPS after the first kill, in seconds of the
 * music, turned into the scene through INTRO_FREEZES; with REDUCE FLASHES,
 * whose scene does not freeze, the hits after the first come that much
 * before their moment of the music. */
[[nodiscard]] float helixHitAt(std::size_t index);

/** @returns The moment of the scene the beams hit newcomer @p index. */
[[nodiscard]] float ringHitAt(std::size_t index);

/** @returns The Bydos of the close-ups at that moment: the Bydo in the dark,
 * the Bydo that crosses the second pilot, the prey of the third, the bonus
 * Bydo that crosses the fourth; a place is empty while its Bydo is not on
 * screen. */
[[nodiscard]] std::array<std::optional<IntroMob>, INTRO_OPENING_MOBS>
openingMobsAt(float scene);

/** @returns Bydo @p index of the helix, from the cut until it dies. */
[[nodiscard]] std::optional<IntroMob> helixMobAt(std::size_t index,
                                                 float scene);

/** @returns Newcomer @p index, from its arrival until it dies. */
[[nodiscard]] std::optional<IntroMob> newcomerMobAt(std::size_t index,
                                                    float scene);

}  // namespace rtype::client

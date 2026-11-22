#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/**
 * @brief The colors of a painted hurt beam, from its line out to its edges:
 * a white core, a pale pink, the pink of the eye of the Bydo Tyrant (sampled
 * from its sheet) and a deep magenta rim.
 */
constexpr sf::Color BEAM_WHITE{255, 255, 255};
constexpr sf::Color BEAM_PALE_PINK{255, 214, 224};
constexpr sf::Color BEAM_PINK{255, 90, 122};
constexpr sf::Color BEAM_DEEP_MAGENTA{140, 31, 69};

/** @brief How far from the line, as a share of half the thickness, each
 * color reaches: the core, the pale pink, the pink; the rim beyond. */
constexpr float BEAM_CORE_SHARE = 0.15F;
constexpr float BEAM_INNER_SHARE = 0.45F;
constexpr float BEAM_BODY_SHARE = 0.85F;

}  // namespace rtype::client

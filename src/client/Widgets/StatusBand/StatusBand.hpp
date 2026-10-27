#pragma once

#include <span>
#include "IInterfaceCanvas.hpp"
#include "PilotStatus.hpp"
#include "PlayerPalette.hpp"

namespace rtype::client {

/**
 * @brief Paints the status band over STATUS_BAND_AREA: an opaque panel cut at
 * its corners, split by thin rules into one bay per player number.
 *
 * The bay of a pilot shows, on its first row, a chip of the pilot's color
 * holding its marker, then its nickname in the light shade of its color, and
 * against the right edge one word, the first that applies: LEFT or SIGNAL
 * LOST once it left, OUT, LAST LIFE, RESPAWN, then YOU for the local player.
 * The nickname keeps the room the word leaves, cut at a whole character; a
 * pilot the waiting room never named shows P and its number. On its second
 * row, STATUS_LIFE_ICON_LIMIT slots of lives, a small ship of its color for
 * each life left and a lost life for each of the others, or one ship and the
 * count past the limit, nothing while they are unknown, the unplugged plug,
 * smoothed, once its signal is lost; then its score on STATUS_SCORE_DIGITS
 * digits, the leading zeros dimmed, stopped at LARGEST_SHOWN_SCORE. A gone
 * pilot keeps its score, dimmed with its chip and its name. The bay of the
 * local player is framed in white.
 *
 * The colors of a pilot are those @p palette gives its number: the chip, the
 * nickname, and the ships and marker of the HUD sheet, whose shades are
 * swapped for those of the palette.
 *
 * A bay whose number has no pilot stays empty. A pilot whose number has no
 * bay, outside FIRST_PLAYER_NUMBER to PLAYER_COLOR_COUNT, is not painted.
 */
void paintStatusBand(IInterfaceCanvas& canvas,
                     std::span<const PilotStatus> pilots,
                     const PlayerPalette& palette);

}  // namespace rtype::client

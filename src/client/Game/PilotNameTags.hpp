#pragma once

#include <span>
#include <string_view>
#include "ComponentRegistry.hpp"
#include "PilotStatus.hpp"
#include "PlayerSettings.hpp"
#include "Position.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

/**
 * @returns How far from the center of a ship its name tag is centered, in
 * logical units: above the ship, its line NAME_TAG_GAP pixels over the top of
 * @p frame, or as far below it when the line would leave the top of the
 * playfield; across from the center of the ship, or moved in as far as the
 * line needs to stay whole within PLAYFIELD_WIDTH.
 *
 * @param ship The center of the ship.
 * @param frame The frame the ship is drawn with.
 * @param text The UTF-8 line of the tag, written in the small font. Its width
 * is taken as NAME_TAG_PIXELS_PER_BYTE per byte, which no line of the small
 * font exceeds, so the line stays whole: at most
 * PLAYFIELD_WIDTH / BACKGROUND_PIXEL_SIZE pixels wide.
 */
[[nodiscard]] game::Position nameTagOffsetOf(game::Position ship,
                                             game::SpriteFrame frame,
                                             std::string_view text);

/**
 * @brief Gives the ship of every pilot of @p pilots its name tag the first time
 * it sees it, then keeps the tag right at each call: shown while PILOT NAMES is
 * on, in the light shade the palette of @p settings gives the pilot, placed by
 * nameTagOffsetOf() from where the ship is now. A ship whose pilot @p pilots
 * does not list gets no tag.
 *
 * The tag is a Caption of the ship itself: RenderSystem writes it from the
 * place the ship is drawn at, in the same frame, and it goes with the ship.
 * Called once per frame, after the ships are placed and before the entities are
 * painted; it allocates only when it gives a ship its tag.
 */
void tagPilotShips(engine::ComponentRegistry& components,
                   std::span<const PilotStatus> pilots,
                   const PlayerSettings& settings);

}  // namespace rtype::client

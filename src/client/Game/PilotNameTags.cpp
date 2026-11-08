#include "PilotNameTags.hpp"
#include <algorithm>
#include <optional>
#include <span>
#include <string_view>
#include "BackgroundConstants.hpp"
#include "Caption.hpp"
#include "ComponentRegistry.hpp"
#include "EffectTargets.hpp"
#include "Entity.hpp"
#include "InterfaceConstants.hpp"
#include "Layer.hpp"
#include "PilotColor.hpp"
#include "PilotName.hpp"
#include "PilotNameTagConstants.hpp"
#include "PilotShips.hpp"
#include "PilotStatus.hpp"
#include "PlayerPreferences.hpp"
#include "PlayerSettings.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "RenderingConstants.hpp"
#include "Sprite.hpp"
#include "SpriteFrame.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

namespace {

/** @returns The pilot of @p pilots flying the ship of a pilot color, or
 * nullptr when none has its number. */
const PilotStatus* pilotOf(std::span<const PilotStatus> pilots,
                           int pilotColor) {
  const int number = playerNumberOfPilotColor(pilotColor);
  const auto pilot =
      std::ranges::find(pilots, number, &PilotStatus::playerNumber);
  return pilot == pilots.end() ? nullptr : &*pilot;
}

/** @returns Half of @p pixels, plus the outline of a line on that side, in
 * logical units: how far an outlined line reaches from its center. */
float outlinedHalfOf(int pixels) {
  return ((static_cast<float>(pixels) * HALF) +
          static_cast<float>(NAME_TAG_OUTLINE_WIDTH)) *
         BACKGROUND_PIXEL_SIZE;
}

/**
 * @returns The name tag of @p pilot: its name as the status band shows it
 * (see pilotNameOf()), in the small font so it does not hide the playfield,
 * outlined so the sky never touches a letter, in the Popups layer, centered
 * on its ship until tagPilotShips() moves it, its color and visibility set by
 * tagPilotShips().
 */
Caption nameTagOf(const PilotStatus& pilot) {
  return Caption{.text = pilotNameOf(pilot.nickname, pilot.playerNumber),
                 .style = TextStyle{.font = TextFont::Small,
                                    .color = {},
                                    .outlined = true,
                                    .alignment = TextAlignment::Center},
                 .layer = game::Layer::Popups,
                 .visible = true};
}

}  // namespace

game::Position nameTagOffsetOf(game::Position ship, game::SpriteFrame frame,
                               std::string_view text) {
  const float rise = captionRiseOver(frame, NAME_TAG_GAP);
  const float halfLine = outlinedHalfOf(TEXT_LINE_HEIGHT);
  const float halfWidth =
      outlinedHalfOf(static_cast<int>(text.size()) * NAME_TAG_PIXELS_PER_BYTE);
  const float center =
      std::min(std::max(ship.x, halfWidth), game::PLAYFIELD_WIDTH - halfWidth);
  const bool fitsAbove = ship.y - rise - halfLine >= 0.0F;
  return game::Position{.x = center - ship.x, .y = fitsAbove ? -rise : rise};
}

void tagPilotShips(engine::ComponentRegistry& components,
                   std::span<const PilotStatus> pilots,
                   const PlayerSettings& settings) {
  const PilotShips found = pilotShipsOf(components);
  for (const engine::Entity ship : std::span(found.ships).first(found.count)) {
    const std::optional<int> pilotColor = pilotColorOf(components, ship);
    const PilotStatus* pilot =
        pilotColor.has_value() ? pilotOf(pilots, *pilotColor) : nullptr;
    const game::Position* place = components.get<game::Position>(ship);
    const game::Sprite* sprite = components.get<game::Sprite>(ship);
    if (pilot == nullptr || place == nullptr || sprite == nullptr) {
      continue;
    }
    if (!components.has<Caption>(ship)) {
      components.add(ship, nameTagOf(*pilot));
    }
    Caption& tag = *components.get<Caption>(ship);
    tag.style.color = pilotLightOf(playerPaletteOf(settings), pilotColor);
    tag.visible = settings.pilotNames;
    tag.offset = nameTagOffsetOf(*place, sprite->frame, tag.text);
  }
}

}  // namespace rtype::client

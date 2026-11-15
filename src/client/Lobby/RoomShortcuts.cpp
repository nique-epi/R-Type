#include "RoomShortcuts.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Event.hpp>
#include <utility>
#include "Button.hpp"
#include "HangarScreenIdConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "KeyChip.hpp"
#include "KeyChipParts.hpp"
#include "LobbyScreenConstants.hpp"
#include "MenuBuilder.hpp"
#include "MenuScreenIdConstants.hpp"
#include "PlayfieldMapping.hpp"
#include "RoomShortcutsParts.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

Button::Text leaderboardText() {
  return {.label = LEADERBOARD_LABEL, .keyName = LEADERBOARD_KEY_NAME};
}

Button::Text hangarText() {
  return {.label = HANGAR_LABEL, .keyName = HANGAR_KEY_NAME};
}

/** @returns Where RANKING lies, ending at @p right. */
sf::IntRect leaderboardChipArea(int right) {
  return MenuBuilder::chipArea(leaderboardText(), right);
}

/** @returns Where HANGAR lies, BUTTON_GAP to the left of RANKING. */
sf::IntRect hangarChipArea(int right) {
  return MenuBuilder::chipArea(
      hangarText(), leaderboardChipArea(right).position.x - BUTTON_GAP);
}

}  // namespace

RoomShortcuts::RoomShortcuts(RoomShortcutsParts parts)
    : navigator_(&parts.navigator.get()),
      beforeOpening_(std::move(parts.beforeOpening)),
      leaderboardChip_(
          KeyChipParts{.text = leaderboardText(),
                       .area = leaderboardChipArea(parts.right),
                       .key = LEADERBOARD_KEY,
                       .action = [this] { open(LEADERBOARD_SCREEN); },
                       .sounds = parts.sounds}),
      hangarChip_(KeyChipParts{.text = hangarText(),
                               .area = hangarChipArea(parts.right),
                               .key = HANGAR_KEY,
                               .action = [this] { open(HANGAR_SCREEN); },
                               .sounds = parts.sounds}) {}

bool RoomShortcuts::handleEvent(const sf::Event& event,
                                const PlayfieldMapping& playfieldPointAt) {
  return leaderboardChip_.handleEvent(event, playfieldPointAt) ||
         hangarChip_.handleEvent(event, playfieldPointAt);
}

void RoomShortcuts::paint(IInterfaceCanvas& canvas) const {
  leaderboardChip_.paint(canvas);
  hangarChip_.paint(canvas);
}

void RoomShortcuts::open(ScreenId screen) {
  beforeOpening_();
  navigator_->push(screen);
}

}  // namespace rtype::client

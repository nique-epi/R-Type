#include "DepartureFeed.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include <vector>
#include "DepartureLine.hpp"
#include "EntitySprites.hpp"
#include "GameNoticeConstants.hpp"
#include "HudSpriteConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "PilotColor.hpp"
#include "PilotName.hpp"
#include "PilotStatus.hpp"
#include "PlayerPalette.hpp"
#include "StatusBandConstants.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"
#include "Typing.hpp"

namespace rtype::client {

namespace {

/** @brief Where a line starts, and the light shade its name is written in. */
struct LinePlace {
  sf::Vector2i topLeft;
  sf::Color nameColor;
};

/** @returns Whether a pilot left the game or lost its signal. */
bool hasLeft(const PilotStatus& pilot) {
  return pilot.state == PilotState::Left ||
         pilot.state == PilotState::SignalLost;
}

/** @brief Paints the icon of a departure in its column: the door for a pilot
 * who left, the plug of the status band for a lost signal. */
void paintIcon(IInterfaceCanvas& canvas, PilotState state,
               sf::Vector2i topLeft) {
  if (state != PilotState::SignalLost) {
    canvas.drawImage(TextureArea{.assetId = HUD_SHEET_ID,
                                 .rectangle = rectangleOf(LEFT_GAME_FRAME)},
                     ImagePlacement{.topLeft = topLeft}, INTERFACE_AREA);
    return;
  }
  canvas.drawSmoothImage(PLUG_ICON_ID,
                         {topLeft + DEPARTURE_PLUG_OFFSET, STATUS_ICON_SIZE},
                         DEPARTURE_WORD_COLOR);
}

/** @brief Paints one line: the icon, the name, then the words. */
void paintLine(IInterfaceCanvas& canvas, const DepartureLine& line,
               const LinePlace& place) {
  paintIcon(canvas, line.state, place.topLeft);
  const sf::Vector2i nameTopLeft =
      place.topLeft + sf::Vector2i(DEPARTURE_ICON_COLUMN + DEPARTURE_PIECE_GAP,
                                   DEPARTURE_TEXT_DROP);
  const std::string_view name = typedPrefix(line.name, DEPARTURE_NAME_LETTERS);
  canvas.write(
      name, nameTopLeft,
      {.font = TextFont::Regular, .color = place.nameColor, .outlined = true});
  const char* words = line.state == PilotState::SignalLost ? SIGNAL_LOST_TEXT
                                                           : LEFT_THE_GAME_TEXT;
  canvas.write(
      words,
      nameTopLeft + sf::Vector2i(canvas.textWidth(name, TextFont::Regular) +
                                     DEPARTURE_WORD_GAP,
                                 0),
      {.font = TextFont::Small,
       .color = DEPARTURE_WORD_COLOR,
       .outlined = true});
}

}  // namespace

void DepartureFeed::follow(std::span<const PilotStatus> pilots) {
  for (const PilotStatus& pilot : pilots) {
    if (isNewDeparture(pilot)) {
      announce(pilot);
    }
  }
}

void DepartureFeed::advance(engine::Duration elapsed) {
  for (DepartureLine& line : lines_) {
    line.age += elapsed;
  }
  const auto expired = std::erase_if(lines_, [](const DepartureLine& line) {
    return line.age >= DEPARTURE_LINE_TIME;
  });
  if (expired > 0) {
    ++revision_;
  }
}

std::span<const DepartureLine> DepartureFeed::lines() const { return lines_; }

std::uint32_t DepartureFeed::revision() const { return revision_; }

void DepartureFeed::paint(IInterfaceCanvas& canvas,
                          const PlayerPalette& palette) const {
  sf::Vector2i topLeft = DEPARTURE_FEED_TOP_LEFT;
  for (const DepartureLine& line : lines_) {
    const auto color =
        static_cast<std::size_t>(pilotColorOfPlayerNumber(line.playerNumber));
    paintLine(canvas, line,
              {.topLeft = topLeft, .nameColor = palette.at(color).light});
    topLeft.y += DEPARTURE_LINE_HEIGHT + DEPARTURE_LINE_GAP;
  }
}

bool DepartureFeed::isNewDeparture(const PilotStatus& pilot) const {
  return hasLeft(pilot) &&
         std::ranges::find(announced_, pilot.playerNumber) == announced_.end();
}

void DepartureFeed::announce(const PilotStatus& pilot) {
  announced_.push_back(pilot.playerNumber);
  lines_.insert(
      lines_.begin(),
      DepartureLine{.playerNumber = pilot.playerNumber,
                    .name = pilotNameOf(pilot.nickname, pilot.playerNumber),
                    .state = pilot.state,
                    .age = engine::Duration::zero()});
  if (lines_.size() > DEPARTURE_FEED_LINES) {
    lines_.pop_back();
  }
  ++revision_;
}

}  // namespace rtype::client

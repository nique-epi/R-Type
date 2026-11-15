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
#include "PilotDeparture.hpp"
#include "PilotName.hpp"
#include "PilotStatus.hpp"
#include "PlayerPalette.hpp"
#include "StatusBandConstants.hpp"
#include "TextMeasures.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @brief Where a line starts, and the light shade its name is written in. */
struct LinePlace {
  sf::Vector2i topLeft;
  sf::Color nameColor;
};

/** @returns The words after the name of a pilot who left in @p state: SIGNAL
 * LOST, WAS EXPELLED, or LEFT THE GAME. */
const char* departureWordsOf(PilotState state) {
  switch (state) {
    case PilotState::SignalLost:
      return SIGNAL_LOST_TEXT;
    case PilotState::Expelled:
      return WAS_EXPELLED_TEXT;
    case PilotState::Left:
    case PilotState::Flying:
    case PilotState::Respawning:
    case PilotState::OutOfLives:
      break;
  }
  return LEFT_THE_GAME_TEXT;
}

/** @brief Paints the icon of a departure in its column: the door for a pilot
 * who left or was expelled, the plug of the status band for a lost signal. */
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

/** @returns Where the words of a line go: after the name, or under it when
 * they would pass DEPARTURE_FEED_RIGHT, as a whole name at a larger text size
 * makes them. */
sf::Vector2i wordsTopLeftOf(const IInterfaceCanvas& canvas,
                            const DepartureLine& line,
                            sf::Vector2i nameTopLeft) {
  const sf::Vector2i after =
      nameTopLeft +
      sf::Vector2i(
          canvas.textWidth(line.name, TextFont::Regular) + DEPARTURE_WORD_GAP,
          0);
  if (after.x +
          canvas.textWidth(departureWordsOf(line.state), TextFont::Small) <=
      DEPARTURE_FEED_RIGHT) {
    return after;
  }
  return {nameTopLeft.x, nameTopLeft.y + lineHeightOf(canvas.textMetrics()) +
                             DEPARTURE_LINE_GAP};
}

/** @brief Paints one line: the icon, the whole name, then the words.
 * @returns How tall the line is: DEPARTURE_LINE_HEIGHT, or more when its
 * text is taller or its words go under the name. */
int paintLine(IInterfaceCanvas& canvas, const DepartureLine& line,
              const LinePlace& place) {
  paintIcon(canvas, line.state, place.topLeft);
  const sf::Vector2i nameTopLeft =
      place.topLeft + sf::Vector2i(DEPARTURE_ICON_COLUMN + DEPARTURE_PIECE_GAP,
                                   DEPARTURE_TEXT_DROP);
  canvas.write(
      line.name, nameTopLeft,
      {.font = TextFont::Regular, .color = place.nameColor, .outlined = true});
  const sf::Vector2i wordsTopLeft = wordsTopLeftOf(canvas, line, nameTopLeft);
  canvas.write(departureWordsOf(line.state), wordsTopLeft,
               {.font = TextFont::Small,
                .color = DEPARTURE_WORD_COLOR,
                .outlined = true});
  return std::max(
      DEPARTURE_LINE_HEIGHT,
      wordsTopLeft.y + lineHeightOf(canvas.textMetrics()) - place.topLeft.y);
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
                          const PlayerPalette& palette,
                          sf::Vector2i topLeft) const {
  for (const DepartureLine& line : lines_) {
    const auto color =
        static_cast<std::size_t>(pilotColorOfPlayerNumber(line.playerNumber));
    topLeft.y +=
        paintLine(canvas, line,
                  {.topLeft = topLeft, .nameColor = palette.at(color).light}) +
        DEPARTURE_LINE_GAP;
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

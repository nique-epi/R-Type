#include "MapVoteCard.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "CutCornerBox.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "InterfacePixel.hpp"
#include "KeyCap.hpp"
#include "KeyDown.hpp"
#include "LobbyScreenConstants.hpp"
#include "LobbyState.hpp"
#include "MapVoteCardParts.hpp"
#include "PlayerPalette.hpp"
#include "PlayfieldMapping.hpp"
#include "ProtocolConstants.hpp"
#include "SoundConstants.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The map with the most votes of @p players among @p mapCount, from
 * 1, the first of them on a tie; nothing when nobody voted. */
std::optional<std::uint8_t> leadingMap(
    std::span<const network::LobbyPlayer> players, std::size_t mapCount) {
  std::optional<std::uint8_t> leading;
  std::ptrdiff_t mostVotes = 0;
  for (std::size_t map = FIRST_MAP_NUMBER; map <= mapCount; ++map) {
    const auto number = static_cast<std::uint8_t>(map);
    const std::ptrdiff_t votes =
        std::ranges::count(players, number, &network::LobbyPlayer::mapVote);
    if (votes > mostVotes) {
      mostVotes = votes;
      leading = number;
    }
  }
  return leading;
}

/** @returns Whether @p event presses the left button inside @p area, read on
 * the grid of the interface, and where. */
std::optional<sf::Vector2i> clickInside(const sf::Event& event,
                                        sf::IntRect area,
                                        const PlayfieldMapping& pointAt) {
  const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
  if (click == nullptr || click->button != sf::Mouse::Button::Left) {
    return std::nullopt;
  }
  const sf::Vector2i pixel = interfacePixelAt(pointAt(click->position));
  return area.contains(pixel) ? std::optional(pixel) : std::nullopt;
}

/** @brief Paints the frame of the preview at @p area, BORDER_THICKNESS wide
 * around it, in @p color. */
void paintPreviewFrame(IInterfaceCanvas& canvas, sf::IntRect area,
                       sf::Color color) {
  const sf::Vector2i border(BORDER_THICKNESS, BORDER_THICKNESS);
  canvas.fill(sf::IntRect(area.position - border,
                          area.size + (border * SIDES_PER_AXIS)),
              color);
}

/** @brief Paints the preview @p asset in @p area; the NO_PREVIEW_MARK on ink
 * without one. */
void paintPreview(IInterfaceCanvas& canvas, sf::IntRect area,
                  const std::optional<std::string>& asset) {
  canvas.fill(area, FIELD_INSIDE_COLOR);
  if (asset.has_value()) {
    canvas.drawImage(TextureArea{.assetId = *asset,
                                 .rectangle = sf::IntRect({0, 0}, area.size)},
                     ImagePlacement{.topLeft = area.position}, area);
    return;
  }
  canvas.write(NO_PREVIEW_MARK,
               area.position + sf::Vector2i(area.size.x / SIDES_PER_AXIS,
                                            (area.size.y - TEXT_LINE_HEIGHT) /
                                                SIDES_PER_AXIS),
               TextStyle{.font = TextFont::Regular,
                         .color = BAR_TEXT_COLOR,
                         .alignment = TextAlignment::Center});
}

/** @brief Paints the MAP_PREVIOUS_MARK left of @p preview and the
 * MAP_NEXT_MARK right of it, on the row of its middle. */
void paintArrows(IInterfaceCanvas& canvas, sf::IntRect preview) {
  const int row = preview.position.y +
                  ((preview.size.y - TEXT_LINE_HEIGHT) / SIDES_PER_AXIS);
  const TextStyle style{.font = TextFont::Small, .color = BAR_TEXT_COLOR};
  canvas.write(MAP_PREVIOUS_MARK, {preview.position.x - MAP_ARROW_GAP, row},
               style);
  canvas.write(MAP_NEXT_MARK,
               {preview.position.x + preview.size.x + MAP_ARROW_GAP -
                    canvas.textWidth(MAP_NEXT_MARK, TextFont::Small),
                row},
               style);
}

/** @returns The lines @p name takes in MAP_NAME_WIDTH pixels of @p font, a
 * word added to a line while it fits, MAP_NAME_LINES at most. */
std::vector<std::string> linesOf(const IInterfaceCanvas& canvas,
                                 const std::string& name, TextFont font) {
  std::vector<std::string> lines;
  std::istringstream words{name};
  std::string word;
  while (words >> word) {
    const std::string joined =
        lines.empty() ? word : lines.back() + MAP_NAME_WORD_SEPARATOR + word;
    if (!lines.empty() && canvas.textWidth(joined, font) <= MAP_NAME_WIDTH) {
      lines.back() = joined;
    } else if (std::cmp_less(lines.size(), MAP_NAME_LINES)) {
      lines.push_back(word);
    }
  }
  return lines;
}

/** @brief Writes @p name from @p topLeft, in the regular font when each of its
 * words fits MAP_NAME_WIDTH, in the small one otherwise. */
void paintName(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
               const std::string& name) {
  std::istringstream words{name};
  std::string word;
  TextFont font = TextFont::Regular;
  while (words >> word) {
    if (canvas.textWidth(word, TextFont::Regular) > MAP_NAME_WIDTH) {
      font = TextFont::Small;
    }
  }
  for (const std::string& line : linesOf(canvas, name, font)) {
    canvas.write(line, topLeft, {.font = font, .color = BAR_TEXT_COLOR});
    topLeft.y += MAP_NAME_LINE_STEP;
  }
}

/** @returns The ship color in @p palette of each of @p players who votes for
 * @p map, in their order; none for NO_MAP_VOTE. */
std::vector<sf::Color> voterColorsOf(
    std::span<const network::LobbyPlayer> players, std::uint8_t map,
    const PlayerPalette& palette) {
  std::vector<sf::Color> colors;
  for (const network::LobbyPlayer& player : players) {
    if (map != network::NO_MAP_VOTE && player.mapVote == map &&
        player.playerNumber >= network::FIRST_PLAYER_NUMBER &&
        player.playerNumber <= palette.size()) {
      colors.push_back(
          palette[player.playerNumber - network::FIRST_PLAYER_NUMBER].ship);
    }
  }
  return colors;
}

/** @brief Paints the keycap of MAP_KEY with its bottom-right corner at
 * @p bottomRight, its edge lit while @p selected. */
void paintKey(IInterfaceCanvas& canvas, sf::Vector2i bottomRight,
              bool selected) {
  const KeyCap key(MAP_KEY_NAME,
                   selected ? KeyCap::Edge::Selected : KeyCap::Edge::Plain);
  key.paint(canvas, bottomRight - sf::Vector2i(key.width(), CONTROL_HEIGHT));
}

/** @brief Paints a pip of each of @p colors from @p topLeft, rightwards. */
void paintPips(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
               std::span<const sf::Color> colors) {
  for (const sf::Color color : colors) {
    canvas.fill(sf::IntRect(topLeft, {VOTE_PIP_SIZE, VOTE_PIP_SIZE}), color);
    topLeft.x += VOTE_PIP_STEP;
  }
}

}  // namespace

MapVoteCard::MapVoteCard(MapVoteCardParts parts)
    : ballot_(&parts.ballot.get()),
      room_(&parts.room.get()),
      sounds_(&parts.sounds.get()),
      previews_(&parts.previews.get()),
      palette_(parts.palette),
      area_(parts.area),
      playfieldPointAt_(std::move(parts.playfieldPointAt)) {}

bool MapVoteCard::handleEvent(const sf::Event& event) {
  if (const std::optional<sf::Vector2i> click =
          isOpen() ? clickInside(event, area_, playfieldPointAt_)
                   : std::nullopt) {
    sounds_->play(CLICK_SOUND_ID);
    click->x < area_.position.x + MAP_PREVIEW_OFFSET.x ? pressBack() : press();
    return true;
  }
  const std::optional<bool> down = keyDownOf(event, MAP_KEY);
  if (!down.has_value()) {
    keyHeld_ = keyHeld_ && !event.is<sf::Event::FocusLost>();
    return false;
  }
  if (*down && !keyHeld_) {
    sounds_->play(CLICK_SOUND_ID);
    press();
  }
  keyHeld_ = *down;
  return true;
}

void MapVoteCard::press() {
  if (!isOpen()) {
    return;
  }
  if (!votedMap().has_value()) {
    ballot_->voteFor(shownMap().value_or(FIRST_MAP_NUMBER));
    return;
  }
  voteAfter(1);
}

void MapVoteCard::pressBack() {
  if (isOpen()) {
    voteAfter(room_->lobbyMaps().size() - 1);
  }
}

bool MapVoteCard::isOpen() const { return !room_->lobbyMaps().empty(); }

std::string MapVoteCard::label() const {
  if (!isOpen()) {
    return MAP_BAR_CLOSED;
  }
  return MAP_BAR_PREFIX + shownName();
}

void MapVoteCard::paint(IInterfaceCanvas& canvas, bool selected) {
  if (!isOpen()) {
    return;
  }
  const sf::Color edge =
      selected ? SELECTED_BAR_BORDER_COLOR : BAR_BORDER_COLOR;
  CutCornerBox(area_, {.border = edge, .inside = BAR_INSIDE_COLOR})
      .paint(canvas);
  const sf::IntRect preview(area_.position + MAP_PREVIEW_OFFSET,
                            MAP_PREVIEW_SIZE);
  const std::optional<std::uint8_t> shown = shownMap();
  paintPreviewFrame(canvas, preview, edge);
  paintPreview(canvas, preview,
               shown.has_value()
                   ? previews_->previewOf(
                         room_->lobbyMaps()[*shown - FIRST_MAP_NUMBER].preview)
                   : std::nullopt);
  paintArrows(canvas, preview);
  const int textLeft = area_.position.x + MAP_CARD_TEXT_LEFT;
  canvas.write(MAP_CARD_LABEL, {textLeft, preview.position.y},
               {.font = TextFont::Small, .color = PAGE_TEXT_COLOR});
  paintName(canvas, {textLeft, area_.position.y + MAP_NAME_TOP}, shownName());
  const sf::Vector2i bottomRight = area_.position + area_.size -
                                   sf::Vector2i(MAP_CARD_INSET, MAP_CARD_INSET);
  paintPips(
      canvas,
      {textLeft,
       bottomRight.y - ((CONTROL_HEIGHT + VOTE_PIP_SIZE) / SIDES_PER_AXIS)},
      voterColorsOf(room_->lobbyPlayers(), shown.value_or(network::NO_MAP_VOTE),
                    palette_.get()));
  paintKey(canvas, bottomRight, selected);
}

std::string MapVoteCard::shownName() const {
  const std::optional<std::uint8_t> shown = shownMap();
  return shown.has_value() ? room_->lobbyMaps()[*shown - FIRST_MAP_NUMBER].name
                           : std::string{NO_MAP_CHOSEN_NAME};
}

std::optional<std::uint8_t> MapVoteCard::shownMap() const {
  const std::optional<std::uint8_t> voted = votedMap();
  return voted.has_value()
             ? voted
             : leadingMap(room_->lobbyPlayers(), room_->lobbyMaps().size());
}

std::optional<std::uint8_t> MapVoteCard::votedMap() const {
  const std::optional<std::uint8_t> requested = ballot_->requestedMap();
  if (!requested.has_value() || *requested == network::NO_MAP_VOTE ||
      *requested > room_->lobbyMaps().size()) {
    return std::nullopt;
  }
  return requested;
}

void MapVoteCard::voteAfter(std::size_t step) {
  const std::size_t count = room_->lobbyMaps().size();
  const std::size_t current = shownMap().value_or(FIRST_MAP_NUMBER);
  ballot_->voteFor(static_cast<std::uint8_t>(
      ((current - FIRST_MAP_NUMBER + step) % count) + FIRST_MAP_NUMBER));
}

}  // namespace rtype::client

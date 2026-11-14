#include "ServerMapPainting.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "LabelCandidate.hpp"
#include "LabelPlacement.hpp"
#include "LabelSide.hpp"
#include "MapView.hpp"
#include "MiniMap.hpp"
#include "PlanetPose.hpp"
#include "SectorLayout.hpp"
#include "SectorMapConstants.hpp"
#include "SectorMapPainting.hpp"
#include "ServerCompatibility.hpp"
#include "ServerListPhase.hpp"
#include "ServerMapChoice.hpp"
#include "ServerMapFrame.hpp"
#include "ServerMapFrames.hpp"
#include "ServerMapLook.hpp"
#include "ServerMapScreenConstants.hpp"
#include "ServerMapStage.hpp"
#include "ServerRow.hpp"
#include "ServerStatus.hpp"
#include "ServerTooltip.hpp"
#include "StarLook.hpp"
#include "StarMark.hpp"
#include "StarName.hpp"
#include "TextStyle.hpp"
#include "TooltipLine.hpp"
#include "WrappedLines.hpp"

namespace rtype::client {

namespace {

StarLook lookOf(const ServerRow& server) {
  return StarLook{.playerCount = server.playerCount,
                  .slow = server.status == network::ServerStatus::Degraded,
                  .incompatible = server.compatibility ==
                                  ServerCompatibility::UpdateNeeded};
}

std::string pingTextOf(const ServerRow& server) {
  if (!server.ping.has_value()) {
    return NO_PING_TEXT;
  }
  return std::to_string(
      std::chrono::duration_cast<std::chrono::milliseconds>(*server.ping)
          .count());
}

/** @returns @p bubble raised so its bottom stays inside the map, above the row
 * of the chips: a star low on the map would push it over them; and moved
 * across, as little as it takes, so its sides stay inside the map, as a bubble
 * grown with its text may not. */
ServerTooltip keptInTheMap(ServerTooltip bubble) {
  const sf::IntRect area = tooltipAreaOf(bubble);
  const int lowestTop =
      SECTOR_MAP_AREA.position.y + SECTOR_MAP_AREA.size.y - area.size.y;
  bubble.topLeft.y = std::min(bubble.topLeft.y, lowestTop);
  const int rightmostLeft =
      SECTOR_MAP_AREA.position.x + SECTOR_MAP_AREA.size.x - area.size.x;
  bubble.topLeft.x = std::max(SECTOR_MAP_AREA.position.x,
                              std::min(bubble.topLeft.x, rightmostLeft));
  return bubble;
}

/** @brief Where a bubble stands: the star of its server, and the stage of the
 * map, which says what ENTER does. */
struct BubbleAt {
  sf::Vector2i star;
  ServerMapStage stage{};
};

/** @returns The bubble of @p server standing at the star of @p place, as wide
 * as its lines need on @p canvas: its pilots of the
 * most it takes and its games, its ping
 * and version, a warning when it is slow or of another version, and what
 * ENTER does at @p stage. */
ServerTooltip tooltipOf(const IInterfaceCanvas& canvas, const ServerRow& server,
                        BubbleAt place) {
  const sf::Vector2i star = place.star;
  std::vector<TooltipLine> lines{
      {.text = std::to_string(server.playerCount) + PILOTS_SEPARATOR +
               std::to_string(server.maximumPlayers) + PILOTS_AND_GAMES_JOIN +
               std::to_string(server.gameCount) + GAMES_WORD,
       .color = PALE_BLUE},
      {.text = pingTextOf(server) + PING_AND_VERSION_JOIN + server.gameVersion,
       .color = PALE_BLUE}};
  if (server.compatibility == ServerCompatibility::UpdateNeeded) {
    lines.push_back({.text = UPDATE_WARNING, .color = STAR_GOLD});
  } else if (server.status == network::ServerStatus::Degraded) {
    lines.push_back({.text = SLOW_WARNING, .color = WARNING_ORANGE});
  }
  lines.push_back({.text = place.stage == ServerMapStage::ZoomedOut
                               ? ZOOM_IN_HINT
                               : OPEN_GAMES_HINT,
                   .color = HUD_CYAN});
  ServerTooltip bubble{.topLeft = {},
                       .width = BUBBLE_WIDTH,
                       .title = server.name,
                       .lines = std::move(lines),
                       .metrics = canvas.textMetrics()};
  bubble.width = tooltipWidthFor(canvas, bubble);
  const bool onTheLeft = star.x > BUBBLE_FLIP_X;
  bubble.topLeft = {onTheLeft ? star.x - BUBBLE_OFFSET.x - bubble.width
                              : star.x + BUBBLE_OFFSET.x,
                    star.y + BUBBLE_OFFSET.y};
  return keptInTheMap(bubble);
}

/** @brief Writes the name of every server that finds a free side, the chosen
 * one served first, away from the boxes @p keepOut. */
void paintNames(IInterfaceCanvas& canvas, const ServerMapChoice& choice,
                const std::vector<sf::IntRect>& keepOut) {
  const std::vector<sf::Vector2i> stars = starPointsOf(choice);
  const std::optional<ServerRow> chosen = choice.chosen();
  std::vector<std::size_t> order;
  for (std::size_t index = 0; index < stars.size(); ++index) {
    const bool isChosen =
        chosen.has_value() && choice.servers()[index].id == chosen->id;
    order.insert(isChosen ? order.begin() : order.end(), index);
  }
  std::vector<LabelCandidate> candidates;
  candidates.reserve(order.size());
  for (const std::size_t index : order) {
    candidates.push_back(LabelCandidate{
        .point = stars[index],
        .nameWidth =
            canvas.textWidth(choice.servers()[index].name, TextFont::Small)});
  }
  const std::vector<LabelSide> sides = placeLabels(candidates, keepOut);
  for (std::size_t place = 0; place < order.size(); ++place) {
    const ServerRow& server = choice.servers()[order[place]];
    paintStarName(
        canvas,
        StarName{.point = stars[order[place]],
                 .name = server.name,
                 .side = sides[place],
                 .chosen = chosen.has_value() && server.id == chosen->id,
                 .incompatible = server.compatibility ==
                                 ServerCompatibility::UpdateNeeded});
  }
}

void paintStars(IInterfaceCanvas& canvas, const ServerMapChoice& choice,
                std::size_t turn) {
  const std::vector<sf::Vector2i> stars = starPointsOf(choice);
  for (std::size_t index = 0; index < stars.size(); ++index) {
    paintStar(canvas, StarMark{.point = stars[index],
                               .look = lookOf(choice.servers()[index]),
                               .turn = turn});
  }
}

/** @brief Paints, when @p chosenShown, the corners and the bubble of the
 * chosen server, and the names that find a free side away from the bubble
 * and, while zoomed in, the minimap. */
void paintNamesAndBubble(IInterfaceCanvas& canvas,
                         const ServerMapChoice& choice, bool chosenShown) {
  std::vector<sf::IntRect> keepOut;
  if (choice.stage() != ServerMapStage::ZoomedOut) {
    keepOut.push_back(MINIMAP_KEEP_OUT);
  }
  const std::optional<ServerRow> chosen = choice.chosen();
  std::optional<ServerTooltip> bubble;
  if (chosen.has_value() && chosenShown) {
    const sf::Vector2i star = placeOf(bearingOf(*chosen), choice.view());
    bubble = tooltipOf(canvas, *chosen,
                       BubbleAt{.star = star, .stage = choice.stage()});
    keepOut.push_back(tooltipAreaOf(*bubble));
    paintChosenStar(canvas, star);
  }
  paintNames(canvas, choice, keepOut);
  if (bubble.has_value()) {
    paintServerTooltip(canvas, *bubble);
  }
}

/** @returns The minimap of @p choice: the planet, a point per server and the
 * part of the map shown, all in pixels of the map zoomed out. */
MiniMap miniMapOf(const ServerMapChoice& choice) {
  const MapView whole{.zoom = ZOOMED_OUT_FACTOR,
                      .centerX = SECTOR_PLANET_CENTER.x,
                      .centerY = SECTOR_PLANET_CENTER.y};
  std::vector<sf::Vector2i> stars;
  for (const ServerRow& server : choice.servers()) {
    stars.push_back(placeOf(bearingOf(server), whole));
  }
  const MapView seen = choice.view();
  const double shrink = whole.zoom / seen.zoom;
  const auto onWhole = [&seen, &whole, shrink](sf::Vector2i point) {
    return sf::Vector2i{
        static_cast<int>(whole.centerX + ((point.x - seen.centerX) * shrink)),
        static_cast<int>(whole.centerY + ((point.y - seen.centerY) * shrink))};
  };
  const sf::Vector2i corner = onWhole(SECTOR_MAP_AREA.position);
  const sf::Vector2i far =
      onWhole(SECTOR_MAP_AREA.position + SECTOR_MAP_AREA.size);
  return MiniMap{.area = MINIMAP_AREA,
                 .planetCenter = SECTOR_PLANET_CENTER,
                 .planetRadius = SECTOR_PLANET_RADIUS,
                 .stars = std::move(stars),
                 .shown = {corner, far - corner}};
}

}  // namespace

void paintServerMapHeader(IInterfaceCanvas& canvas, std::size_t serverCount) {
  canvas.write(
      SERVER_MAP_TITLE, SERVER_MAP_TITLE_ANCHOR,
      TextStyle{
          .font = TextFont::Regular, .color = HUD_CYAN, .outlined = true});
  const std::string subtitle =
      (serverCount == 1 ? std::string{ONE_SERVER_TEXT}
                        : std::to_string(serverCount) + SERVER_COUNT_SUFFIX) +
      SERVER_MAP_HINT;
  const TextStyle style{
      .font = TextFont::Small, .color = PALE_BLUE, .outlined = true};
  const ServerMapFrame frame = serverMapFrameOf(canvas.textMetrics());
  writeWrappedLines(
      canvas, subtitle,
      {.anchor = {SERVER_MAP_SUBTITLE_ANCHOR.x, frame.subtitleTop},
       .width = frame.subtitleWidth,
       .style = style});
}

void paintServerMapMessage(IInterfaceCanvas& canvas, ServerListPhase phase) {
  const auto lines =
      [phase]() -> std::optional<std::pair<const char*, const char*>> {
    switch (phase) {
      case ServerListPhase::Searching:
        return std::pair{SEARCHING_TITLE, SEARCHING_DETAIL};
      case ServerListPhase::Empty:
        return std::pair{NO_SERVER_TITLE, NO_SERVER_DETAIL};
      case ServerListPhase::MasterUnreachable:
        return std::pair{MASTER_UNREACHABLE_TITLE, MASTER_UNREACHABLE_DETAIL};
      case ServerListPhase::Ready:
        break;
    }
    return std::nullopt;
  }();
  if (!lines.has_value()) {
    return;
  }
  canvas.write(
      lines->first, MESSAGE_ANCHOR,
      TextStyle{.font = TextFont::Regular,
                .color = phase == ServerListPhase::MasterUnreachable ? STAR_GOLD
                                                                     : HUD_CYAN,
                .alignment = TextAlignment::Center});
  canvas.write(lines->second,
               MESSAGE_ANCHOR + sf::Vector2i{0, MESSAGE_LINE_PITCH},
               TextStyle{.font = TextFont::Small,
                         .color = PALE_BLUE,
                         .alignment = TextAlignment::Center});
}

PlanetPose planetPoseIn(const MapView& view, PlanetPose turned) {
  turned.center = {static_cast<int>(view.centerX),
                   static_cast<int>(view.centerY)};
  turned.radius = static_cast<int>(SECTOR_PLANET_RADIUS * view.zoom);
  return turned;
}

void paintServerMap(IInterfaceCanvas& canvas, const ServerMapChoice& choice,
                    ServerMapLook look) {
  const MapView view = choice.view();
  paintPingRings(canvas, view);
  paintStars(canvas, choice, look.turn);
  const bool zoomedIn = choice.stage() != ServerMapStage::ZoomedOut;
  paintNamesAndBubble(canvas, choice, look.chosenShown || zoomedIn);
  if (zoomedIn) {
    paintMiniMap(canvas, miniMapOf(choice));
  }
  paintSectorFrame(canvas);
}

}  // namespace rtype::client

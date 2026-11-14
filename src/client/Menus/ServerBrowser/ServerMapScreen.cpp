#include "ServerMapScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include "Ambience.hpp"
#include "BarState.hpp"
#include "Button.hpp"
#include "ChosenGame.hpp"
#include "ChosenGameConnection.hpp"
#include "ConnectionRequest.hpp"
#include "Endpoint.hpp"
#include "GameRequest.hpp"
#include "GameRow.hpp"
#include "IGameCatalog.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "InterfacePixel.hpp"
#include "KeyChip.hpp"
#include "KeyChipParts.hpp"
#include "MapDirection.hpp"
#include "MapPlaceConstants.hpp"
#include "MapPlaces.hpp"
#include "MenuBuilder.hpp"
#include "MenuScreenIdConstants.hpp"
#include "PlanetPose.hpp"
#include "PlayfieldMapping.hpp"
#include "RingedPlanet.hpp"
#include "RowScrolling.hpp"
#include "SectorMapPainting.hpp"
#include "ServerCompatibility.hpp"
#include "ServerListPhase.hpp"
#include "ServerMapChoice.hpp"
#include "ServerMapFrame.hpp"
#include "ServerMapFrames.hpp"
#include "ServerMapLook.hpp"
#include "ServerMapPainting.hpp"
#include "ServerMapScreenConstants.hpp"
#include "ServerMapScreenParts.hpp"
#include "ServerMapStage.hpp"
#include "ServerRow.hpp"
#include "ShiftedCanvas.hpp"
#include "TextMetrics.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief Where the chips of the map lie: the frame of the map, which gives
 * the tops of their rows, and the text size of their labels. */
struct ChipRows {
  ServerMapFrame frame;
  TextMetrics metrics;
};

Button::Text quickPlayText(TextMetrics metrics) {
  return {.label = QUICK_PLAY_LABEL,
          .keyName = QUICK_PLAY_KEY_NAME,
          .size = metrics};
}

Button::Text rankingText(TextMetrics metrics) {
  return {.label = RANKING_LABEL, .keyName = RANKING_KEY_NAME, .size = metrics};
}

Button::Text addressText(TextMetrics metrics) {
  return {.label = ADDRESS_LABEL, .keyName = ADDRESS_KEY_NAME, .size = metrics};
}

Button::Text backText(TextMetrics metrics) {
  return {.label = BACK_LABEL, .keyName = CANCEL_KEY_NAME, .size = metrics};
}

/** @returns Where a chip with @p text lies, its top-right corner at
 * @p topRight. */
sf::IntRect chipAreaEndingAt(const Button::Text& text, sf::Vector2i topRight) {
  sf::IntRect area = MenuBuilder::chipArea(text, topRight.x);
  area.position.y = topRight.y;
  return area;
}

/** @returns Where BACK lies: at the right end of the lower row. */
sf::IntRect backChipArea(const ChipRows& rows) {
  return chipAreaEndingAt(backText(rows.metrics),
                          {SERVER_MAP_CHIPS_RIGHT, rows.frame.lowerChipsTop});
}

/** @returns Where ADDRESS lies: just left of BACK. */
sf::IntRect addressChipArea(const ChipRows& rows) {
  return chipAreaEndingAt(
      addressText(rows.metrics),
      {backChipArea(rows).position.x - BUTTON_GAP, rows.frame.lowerChipsTop});
}

/** @returns Where RANKING lies: just left of ADDRESS while the chips take one
 * row; at the right end of the upper row once they take two. */
sf::IntRect rankingChipArea(const ChipRows& rows) {
  const bool oneRow = rows.frame.upperChipsTop == rows.frame.lowerChipsTop;
  return chipAreaEndingAt(
      rankingText(rows.metrics),
      {oneRow ? addressChipArea(rows).position.x - BUTTON_GAP
              : SERVER_MAP_CHIPS_RIGHT,
       rows.frame.upperChipsTop});
}

/** @returns Where QUICK PLAY lies: alone on the left of the upper row, the
 * main action. */
sf::IntRect quickPlayChipArea(const ChipRows& rows) {
  sf::IntRect area =
      chipAreaEndingAt(quickPlayText(rows.metrics),
                       {SERVER_MAP_CHIPS_RIGHT, rows.frame.upperChipsTop});
  area.position.x = SERVER_MAP_CHIPS_LEFT;
  return area;
}

/** @returns The direction an arrow key moves the choice towards. */
std::optional<MapDirection> directionOf(sf::Keyboard::Key key) {
  switch (key) {
    case sf::Keyboard::Key::Up:
      return MapDirection::Up;
    case sf::Keyboard::Key::Down:
      return MapDirection::Down;
    case sf::Keyboard::Key::Left:
      return MapDirection::Left;
    case sf::Keyboard::Key::Right:
      return MapDirection::Right;
    default:
      return std::nullopt;
  }
}

/** @returns Whether QUICK PLAY may send a player to @p server: it is of this
 * version and the master lists fewer players on it than its games hold. The
 * count is the one of its last heartbeat, so the server may still refuse. */
bool takesOneMorePlayer(const ServerRow& server) noexcept {
  return server.compatibility == ServerCompatibility::Compatible &&
         server.playerCount < server.maximumPlayers;
}

/** @returns Any game of the server with the lowest ping that
 * takesOneMorePlayer(): the first one of a list sorted by ping, with the
 * others that do, in the same order, to try when it answers that it is full
 * after all. Nothing when no server does. */
std::optional<ChosenGame> quickPlayGameOf(
    const std::vector<ServerRow>& servers) {
  std::vector<network::Endpoint> open;
  for (const ServerRow& server : servers) {
    if (takesOneMorePlayer(server)) {
      open.push_back(server.endpoint);
    }
  }
  if (open.empty()) {
    return std::nullopt;
  }
  return ChosenGame{.server = open.front(),
                    .request = gameRequestFor(network::ANY_GAME_CHOICE),
                    .otherServers = {open.begin() + 1, open.end()}};
}

/** @returns The rows an arrow moves the choice of a game by: one up, one
 * down, none across. */
int rowStepOf(MapDirection direction) {
  switch (direction) {
    case MapDirection::Up:
      return -1;
    case MapDirection::Down:
      return 1;
    case MapDirection::Left:
    case MapDirection::Right:
      break;
  }
  return 0;
}

/** @brief Opens the games of the server @p map chose, from any stage, and
 * asks @p games for them; nothing without a chosen server. */
void openGamesOf(ServerMapChoice& map, IGameCatalog& games) {
  const std::optional<ServerRow> chosen = map.chosen();
  if (!chosen.has_value()) {
    return;
  }
  if (map.stage() == ServerMapStage::ZoomedOut) {
    map.deeper();
  }
  if (map.stage() == ServerMapStage::ZoomedIn) {
    map.deeper();
    games.ask(*chosen);
  }
}

/** @returns The place of MAP_PLACES that @p pixel picks: only the whole map
 * shows them. */
std::optional<std::size_t> placeShownAt(const ServerMapChoice& map,
                                        sf::Vector2i pixel) {
  return map.stage() == ServerMapStage::ZoomedOut ? mapPlaceAt(pixel)
                                                  : std::nullopt;
}

/** @brief Lets go of @p heldKey, then pushes the screen of the place
 * @p place over the map: the release of that key goes to that screen. */
void openPlace(std::optional<sf::Keyboard::Key>& heldKey,
               IScreenNavigator& navigator, std::size_t place) {
  heldKey.reset();
  navigator.push(MAP_PLACES.at(place).screen);
}

/** @brief Zooms in when the wheel turns up over the whole map, out when it
 * turns down over the zoomed map. */
void zoomWithWheel(ServerMapChoice& map, float delta) {
  if (delta > 0.0F && map.stage() == ServerMapStage::ZoomedOut) {
    map.deeper();
  } else if (delta < 0.0F && map.stage() == ServerMapStage::ZoomedIn) {
    static_cast<void>(map.shallower());
  }
}

/** @returns The request that joins @p game, with the rules the master lists
 * for it. */
GameRequest requestToJoin(const GameRow& game) {
  return GameRequest{
      .game = network::GameChoice{.kind = network::GameChoiceKind::ExistingGame,
                                  .number = game.number},
      .rules = game.rules};
}

KeyChipParts quickPlayChipParts(const ChipRows& rows,
                                std::function<void()> action,
                                ISoundPlayer& sounds) {
  return KeyChipParts{.text = quickPlayText(rows.metrics),
                      .area = quickPlayChipArea(rows),
                      .key = QUICK_PLAY_KEY,
                      .action = std::move(action),
                      .sounds = sounds,
                      .restState = BarState::Selected};
}

KeyChipParts rankingChipParts(const ChipRows& rows,
                              std::function<void()> action,
                              ISoundPlayer& sounds) {
  return KeyChipParts{.text = rankingText(rows.metrics),
                      .area = rankingChipArea(rows),
                      .key = RANKING_KEY,
                      .action = std::move(action),
                      .sounds = sounds};
}

KeyChipParts addressChipParts(const ChipRows& rows,
                              std::function<void()> action,
                              ISoundPlayer& sounds) {
  return KeyChipParts{.text = addressText(rows.metrics),
                      .area = addressChipArea(rows),
                      .key = ADDRESS_KEY,
                      .action = std::move(action),
                      .sounds = sounds};
}

KeyChipParts backChipParts(const ChipRows& rows, std::function<void()> action,
                           ISoundPlayer& sounds) {
  return KeyChipParts{.text = backText(rows.metrics),
                      .area = backChipArea(rows),
                      .key = MENU_CANCEL_KEY,
                      .action = std::move(action),
                      .sounds = sounds};
}

}  // namespace

ServerMapScreen::ServerMapScreen(ServerMapScreenParts parts,
                                 std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      ambience_(parts.audio.get(), Ambience::Space),
      servers_(&parts.servers.get()),
      games_(&parts.games.get()),
      choice_(&parts.choice.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      frame_(serverMapFrameOf(canvas_->textMetrics())),
      motion_(parts.motion),
      panel_(canvas_->textMetrics()),
      sessionEnd_(parts.connection.get()),
      quickPlayChip_(quickPlayChipParts(
          {.frame = frame_, .metrics = canvas_->textMetrics()},
          [this] {
            if (const std::optional<ChosenGame> game =
                    quickPlayGameOf(map_.servers())) {
              heldKey_.reset();
              openConnectionTo(*choice_, *game, *navigator_);
            }
          },
          parts.sounds)),
      rankingChip_(rankingChipParts(
          {.frame = frame_, .metrics = canvas_->textMetrics()},
          [this] { navigator_->push(LEADERBOARD_SCREEN); }, parts.sounds)),
      addressChip_(addressChipParts(
          {.frame = frame_, .metrics = canvas_->textMetrics()},
          [this] { navigator_->push(CONNECTION_SCREEN); }, parts.sounds)),
      backChip_(backChipParts(
          {.frame = frame_, .metrics = canvas_->textMetrics()},
          [this] { stepBack(); }, parts.sounds)) {
  servers_->refresh();
  repaint();
}

void ServerMapScreen::handleEvent(const sf::Event& event) {
  const auto pagePointAt = shiftedMappingOf({0, scroll_}, playfieldPointAt_);
  const auto mapPointAt =
      shiftedMappingOf({0, scroll_ - frame_.mapShift}, playfieldPointAt_);
  scroll_ = pagedScrollOf(event,
                          {.scroll = scroll_, .last = lastMapScrollOf(frame_)});
  quickPlayChip_.handleEvent(event, pagePointAt);
  rankingChip_.handleEvent(event, pagePointAt);
  addressChip_.handleEvent(event, pagePointAt);
  backChip_.handleEvent(event, pagePointAt);
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    keyPressed(*pressed);
  } else if (event.is<sf::Event::KeyReleased>() ||
             event.is<sf::Event::FocusLost>()) {
    heldKey_.reset();
  } else if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
    const sf::Vector2i pixel = interfacePixelAt(mapPointAt(moved->position));
    if (map_.stage() == ServerMapStage::Games) {
      static_cast<void>(panel_.chooseAt(pixel));
    }
    placeShown_ = placeShownAt(map_, pixel);
    chosenShown_ = !placeShown_ && map_.chooseAt(pixel);
  } else if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
             click != nullptr && click->button == sf::Mouse::Button::Left) {
    pointerPressed(interfacePixelAt(mapPointAt(click->position)));
  } else if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>()) {
    zoomWithWheel(map_, wheel->delta);
  }
}

void ServerMapScreen::update(engine::Duration elapsed) {
  elapsed_ += elapsed;
  sinceRefresh_ += elapsed;
  const bool cameBack = sessionEnd_.recheckDue(elapsed);
  if (sinceRefresh_ >= SERVER_LIST_REFRESH_INTERVAL || cameBack) {
    refresh();
  }
  if (servers_->snapshot().servers != map_.servers()) {
    map_.showServers(servers_->snapshot().servers);
  }
  if (const std::optional<ServerRow> chosen = map_.chosen();
      chosen.has_value() && map_.stage() == ServerMapStage::Games) {
    panel_.show(games_->snapshot(), chosen->name);
  }
  quickPlayChip_.setEnabled(quickPlayGameOf(map_.servers()).has_value());
  repaint();
}

void ServerMapScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool ServerMapScreen::hidesScreensBelow() const { return true; }

void ServerMapScreen::keyPressed(const sf::Event::KeyPressed& key) {
  if (const std::optional<MapDirection> direction = directionOf(key.code)) {
    if (map_.stage() == ServerMapStage::Games) {
      panel_.move(rowStepOf(*direction));
    } else {
      map_.moveTowards(*direction);
      chosenShown_ = true;
    }
    return;
  }
  if (key.code == sf::Keyboard::Key::Tab &&
      map_.stage() != ServerMapStage::Games) {
    map_.moveByPing(key.shift ? -1 : 1);
    chosenShown_ = true;
    return;
  }
  if (const auto place = mapPlaceOpenedBy(key.code)) {
    openPlace(heldKey_, *navigator_, *place);
    return;
  }
  if ((key.code != MENU_CONFIRM_KEY && key.code != REFRESH_KEY) ||
      heldKey_ == key.code) {
    return;
  }
  heldKey_ = key.code;
  if (key.code == REFRESH_KEY) {
    refresh();
  } else {
    confirm();
  }
}

void ServerMapScreen::confirm() {
  const std::optional<ServerRow> chosen = map_.chosen();
  if (!chosen.has_value()) {
    return;
  }
  switch (map_.stage()) {
    case ServerMapStage::ZoomedOut:
      map_.deeper();
      return;
    case ServerMapStage::ZoomedIn:
      openGamesOf(map_, *games_);
      return;
    case ServerMapStage::Games:
      break;
  }
  if (panel_.newGameChosen()) {
    heldKey_.reset();
    navigator_->push(NEW_GAME_SCREEN);
  } else if (const std::optional<GameRow> game = panel_.joinable()) {
    heldKey_.reset();
    openConnectionTo(*choice_,
                     ChosenGame{.server = chosen->endpoint,
                                .request = requestToJoin(*game),
                                .otherServers = {}},
                     *navigator_);
  }
}

void ServerMapScreen::pointerPressed(sf::Vector2i pixel) {
  if (map_.stage() == ServerMapStage::Games) {
    if (panel_.chooseAt(pixel)) {
      confirm();
    }
  } else if (const auto place = placeShownAt(map_, pixel)) {
    openPlace(heldKey_, *navigator_, *place);
  } else if (map_.chooseAt(pixel)) {
    openGamesOf(map_, *games_);
  }
}

void ServerMapScreen::stepBack() {
  if (!map_.shallower()) {
    navigator_->pop();
  }
}

void ServerMapScreen::refresh() {
  sinceRefresh_ = engine::Duration::zero();
  servers_->refresh();
  const std::optional<ServerRow> chosen = map_.chosen();
  if (map_.stage() == ServerMapStage::Games && chosen.has_value()) {
    games_->ask(*chosen);
  }
}

void ServerMapScreen::repaint() {
  canvas_->clear();
  ShiftedCanvas page(*canvas_, {0, -scroll_});
  ShiftedCanvas sector(*canvas_, {0, frame_.mapShift - scroll_});
  const ServerListPhase phase = servers_->snapshot().phase;
  paintServerMapHeader(page, map_.servers().size());
  if (phase == ServerListPhase::Ready) {
    const PlanetPose pose = planetPoseIn(
        map_.view(), turnedPlanet(PlanetPose{}, elapsed_, motion_));
    if (pose != planetPose_) {
      planetPixels_ = sectorPlanetPixels(pose);
      planetPose_ = pose;
    }
    sector.plot(planetPixels_);
    paintServerMap(sector, map_,
                   ServerMapLook{.chosenShown = chosenShown_,
                                 .turn = planetTurnAfter(elapsed_, motion_)});
    if (map_.stage() == ServerMapStage::Games) {
      panel_.paint(sector);
    }
  } else {
    paintServerMapMessage(sector, phase);
  }
  if (map_.stage() == ServerMapStage::ZoomedOut) {
    paintMapPlaces(sector, chosenShown_ ? std::nullopt : placeShown_);
  }
  for (const KeyChip* chip :
       {&quickPlayChip_, &rankingChip_, &addressChip_, &backChip_}) {
    chip->paint(page);
  }
  paintPictureScrollMarks(*canvas_,
                          {.scroll = scroll_, .last = lastMapScrollOf(frame_)});
  canvas_->display();
}

}  // namespace rtype::client

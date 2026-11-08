#include "ServerMapScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include "Ambience.hpp"
#include "BarState.hpp"
#include "Button.hpp"
#include "ConnectionRequest.hpp"
#include "GameRequest.hpp"
#include "GameRow.hpp"
#include "HangarScreenIdConstants.hpp"
#include "HangarStar.hpp"
#include "IGameCatalog.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "InterfacePixel.hpp"
#include "KeyChip.hpp"
#include "KeyChipParts.hpp"
#include "MapDirection.hpp"
#include "MenuBuilder.hpp"
#include "MenuScreenIdConstants.hpp"
#include "PlanetPose.hpp"
#include "RingedPlanet.hpp"
#include "SectorMapPainting.hpp"
#include "ServerCompatibility.hpp"
#include "ServerListPhase.hpp"
#include "ServerMapChoice.hpp"
#include "ServerMapLook.hpp"
#include "ServerMapPainting.hpp"
#include "ServerMapScreenConstants.hpp"
#include "ServerMapScreenParts.hpp"
#include "ServerMapStage.hpp"
#include "ServerRow.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

Button::Text quickPlayText() {
  return {.label = QUICK_PLAY_LABEL, .keyName = QUICK_PLAY_KEY_NAME};
}

Button::Text rankingText() {
  return {.label = RANKING_LABEL, .keyName = RANKING_KEY_NAME};
}

Button::Text addressText() {
  return {.label = ADDRESS_LABEL, .keyName = ADDRESS_KEY_NAME};
}

Button::Text backText() {
  return {.label = BACK_LABEL, .keyName = CANCEL_KEY_NAME};
}

/** @returns Where a chip with @p text lies, ending at @p right on the row of
 * the chips of the map. */
sf::IntRect chipAreaEndingAt(const Button::Text& text, int right) {
  sf::IntRect area = MenuBuilder::chipArea(text, right);
  area.position.y = SERVER_MAP_CHIPS_TOP;
  return area;
}

/** @returns Where the chip of @p text lies, just left of @p rightNeighbour. */
sf::IntRect chipAreaLeftOf(const Button::Text& text,
                           sf::IntRect rightNeighbour) {
  return chipAreaEndingAt(text, rightNeighbour.position.x - BUTTON_GAP);
}

sf::IntRect backChipArea() {
  return chipAreaEndingAt(backText(), SERVER_MAP_CHIPS_RIGHT);
}

sf::IntRect addressChipArea() {
  return chipAreaLeftOf(addressText(), backChipArea());
}

sf::IntRect rankingChipArea() {
  return chipAreaLeftOf(rankingText(), addressChipArea());
}

/** @returns Where QUICK PLAY lies: alone on the left, the main action. */
sf::IntRect quickPlayChipArea() {
  sf::IntRect area = chipAreaEndingAt(quickPlayText(), SERVER_MAP_CHIPS_RIGHT);
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

/** @returns The server of this version with the lowest ping: the first one of
 * a list sorted by ping. */
std::optional<ServerRow> quickPlayServerOf(
    const std::vector<ServerRow>& servers) {
  const auto found = std::ranges::find(servers, ServerCompatibility::Compatible,
                                       &ServerRow::compatibility);
  if (found == servers.end()) {
    return std::nullopt;
  }
  return *found;
}

/** @returns Whether a server of @p servers is of this version. */
bool holdsAServerOfThisVersion(const std::vector<ServerRow>& servers) {
  return std::ranges::any_of(servers, [](const ServerRow& server) noexcept {
    return server.compatibility == ServerCompatibility::Compatible;
  });
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

/** @returns Whether @p pixel picks the hangar star: only the whole map
 * shows it. */
bool hangarStarShownAt(const ServerMapChoice& map, sf::Vector2i pixel) {
  return map.stage() == ServerMapStage::ZoomedOut && hangarStarAt(pixel);
}

/** @brief Lets go of @p heldKey, then pushes the hangar over the map: the
 * release of that key goes to the hangar. */
void openHangar(std::optional<sf::Keyboard::Key>& heldKey,
                IScreenNavigator& navigator) {
  heldKey.reset();
  navigator.push(HANGAR_SCREEN);
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

KeyChipParts quickPlayChipParts(std::function<void()> action,
                                ISoundPlayer& sounds) {
  return KeyChipParts{.text = quickPlayText(),
                      .area = quickPlayChipArea(),
                      .key = QUICK_PLAY_KEY,
                      .action = std::move(action),
                      .sounds = sounds,
                      .restState = BarState::Selected};
}

KeyChipParts rankingChipParts(std::function<void()> action,
                              ISoundPlayer& sounds) {
  return KeyChipParts{.text = rankingText(),
                      .area = rankingChipArea(),
                      .key = RANKING_KEY,
                      .action = std::move(action),
                      .sounds = sounds};
}

KeyChipParts addressChipParts(std::function<void()> action,
                              ISoundPlayer& sounds) {
  return KeyChipParts{.text = addressText(),
                      .area = addressChipArea(),
                      .key = ADDRESS_KEY,
                      .action = std::move(action),
                      .sounds = sounds};
}

KeyChipParts backChipParts(std::function<void()> action, ISoundPlayer& sounds) {
  return KeyChipParts{.text = backText(),
                      .area = backChipArea(),
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
      motion_(parts.motion),
      quickPlayChip_(quickPlayChipParts(
          [this] {
            if (const std::optional<ServerRow> server =
                    quickPlayServerOf(map_.servers())) {
              join(*server, gameRequestFor(network::ANY_GAME_CHOICE));
            }
          },
          parts.sounds)),
      rankingChip_(rankingChipParts(
          [this] { navigator_->push(LEADERBOARD_SCREEN); }, parts.sounds)),
      addressChip_(addressChipParts(
          [this] { navigator_->push(CONNECTION_SCREEN); }, parts.sounds)),
      backChip_(backChipParts([this] { stepBack(); }, parts.sounds)) {
  servers_->refresh();
  repaint();
}

void ServerMapScreen::handleEvent(const sf::Event& event) {
  quickPlayChip_.handleEvent(event, playfieldPointAt_);
  rankingChip_.handleEvent(event, playfieldPointAt_);
  addressChip_.handleEvent(event, playfieldPointAt_);
  backChip_.handleEvent(event, playfieldPointAt_);
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    keyPressed(*pressed);
  } else if (event.is<sf::Event::KeyReleased>() ||
             event.is<sf::Event::FocusLost>()) {
    heldKey_.reset();
  } else if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
    const sf::Vector2i pixel =
        interfacePixelAt(playfieldPointAt_(moved->position));
    hangarShown_ = hangarStarShownAt(map_, pixel);
    chosenShown_ = !hangarShown_ && map_.chooseAt(pixel);
  } else if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
             click != nullptr && click->button == sf::Mouse::Button::Left) {
    const sf::Vector2i pixel =
        interfacePixelAt(playfieldPointAt_(click->position));
    if (hangarStarShownAt(map_, pixel)) {
      openHangar(heldKey_, *navigator_);
    } else if (map_.chooseAt(pixel)) {
      openGamesOf(map_, *games_);
    }
  } else if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>()) {
    zoomWithWheel(map_, wheel->delta);
  }
}

void ServerMapScreen::update(engine::Duration elapsed) {
  elapsed_ += elapsed;
  sinceRefresh_ += elapsed;
  if (sinceRefresh_ >= SERVER_LIST_REFRESH_INTERVAL) {
    refresh();
  }
  if (servers_->snapshot().servers != map_.servers()) {
    map_.showServers(servers_->snapshot().servers);
  }
  if (const std::optional<ServerRow> chosen = map_.chosen();
      chosen.has_value() && map_.stage() == ServerMapStage::Games) {
    panel_.show(games_->snapshot(), chosen->name);
  }
  quickPlayChip_.setEnabled(holdsAServerOfThisVersion(map_.servers()));
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
  if (key.code == HANGAR_KEY) {
    openHangar(heldKey_, *navigator_);
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
    join(*chosen, requestToJoin(*game));
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

void ServerMapScreen::join(const ServerRow& server,
                           const GameRequest& request) {
  heldKey_.reset();
  choice_->lastServer = server.endpoint;
  choice_->lastRequest = request;
  navigator_->push(CONNECTION_TO_MAP_SERVER_SCREEN);
}

void ServerMapScreen::repaint() {
  canvas_->clear();
  const ServerListPhase phase = servers_->snapshot().phase;
  paintServerMapHeader(*canvas_, map_.servers().size());
  if (phase == ServerListPhase::Ready) {
    const PlanetPose pose = planetPoseIn(
        map_.view(), turnedPlanet(PlanetPose{}, elapsed_, motion_));
    if (pose != planetPose_) {
      planetPixels_ = sectorPlanetPixels(pose);
      planetPose_ = pose;
    }
    canvas_->plot(planetPixels_);
    paintServerMap(*canvas_, map_,
                   ServerMapLook{.chosenShown = chosenShown_,
                                 .turn = planetTurnAfter(elapsed_, motion_)});
    if (map_.stage() == ServerMapStage::Games) {
      panel_.paint(*canvas_);
    }
  } else {
    paintServerMapMessage(*canvas_, phase);
  }
  if (map_.stage() == ServerMapStage::ZoomedOut) {
    paintHangarStar(*canvas_, hangarShown_ && !chosenShown_);
  }
  quickPlayChip_.paint(*canvas_);
  rankingChip_.paint(*canvas_);
  addressChip_.paint(*canvas_);
  backChip_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client

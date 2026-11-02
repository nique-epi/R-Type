#include "LeaderboardScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "Animation.hpp"
#include "BarState.hpp"
#include "Button.hpp"
#include "CutCornerBox.hpp"
#include "EntitySprites.hpp"
#include "IInterfaceCanvas.hpp"
#include "ILeaderboardSource.hpp"
#include "ISoundPlayer.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "InterfacePixel.hpp"
#include "ItemList.hpp"
#include "KeyChipParts.hpp"
#include "KeyNames.hpp"
#include "LeaderboardFigure.hpp"
#include "LeaderboardFilterChoice.hpp"
#include "LeaderboardMessage.hpp"
#include "LeaderboardPage.hpp"
#include "LeaderboardPhase.hpp"
#include "LeaderboardPresentation.hpp"
#include "LeaderboardScreenConstants.hpp"
#include "LeaderboardScreenParts.hpp"
#include "LeaderboardSpriteConstants.hpp"
#include "LeaderboardTab.hpp"
#include "MenuBuilder.hpp"
#include "MenuScreenIdConstants.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenConstants.hpp"
#include "SoundConstants.hpp"
#include "SpriteFrame.hpp"
#include "TabStrip.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Where a chip with this text lies: ending at @p right, on the row of
 * the chips, as wide as MenuBuilder makes a chip. */
sf::IntRect chipAreaEndingAt(const Button::Text& text, int right) {
  sf::IntRect area = MenuBuilder::chipArea(text);
  area.position = {right - area.size.x, LEADERBOARD_CHIPS_TOP};
  return area;
}

sf::IntRect backChipArea() {
  return chipAreaEndingAt({.label = BACK_LABEL, .keyName = CANCEL_KEY_NAME},
                          LEADERBOARD_CHIPS_RIGHT);
}

sf::IntRect filterChipArea() {
  return chipAreaEndingAt({.label = FILTER_LABEL, .keyName = FILTER_KEY_NAME},
                          backChipArea().position.x - BUTTON_GAP);
}

KeyChipParts backChipParts(std::function<void()> action, ISoundPlayer& sounds) {
  return KeyChipParts{.text = {.label = BACK_LABEL, .keyName = CANCEL_KEY_NAME},
                      .area = backChipArea(),
                      .key = MENU_CANCEL_KEY,
                      .action = std::move(action),
                      .sounds = sounds};
}

KeyChipParts filterChipParts(std::function<void()> action,
                             ISoundPlayer& sounds) {
  return KeyChipParts{
      .text = {.label = FILTER_LABEL, .keyName = FILTER_KEY_NAME},
      .area = filterChipArea(),
      .key = FILTER_KEY,
      .action = std::move(action),
      .sounds = sounds};
}

/** @returns The bar under NOT_ON_SERVER_MESSAGE, selected: the one thing the
 * screen asks of the player then. */
KeyChipParts joinBarParts(std::function<void()> action, ISoundPlayer& sounds) {
  return KeyChipParts{.text = {.label = JOIN_LABEL},
                      .area = JOIN_BAR,
                      .key = MENU_CONFIRM_KEY,
                      .action = std::move(action),
                      .sounds = sounds,
                      .restState = BarState::Selected};
}

/** @returns The strip of these tabs, one of them open, as wide as the panel. */
TabStrip tabStripOf(const std::vector<LeaderboardTab>& tabs, std::size_t open,
                    const KeyNaming& keyName) {
  std::vector<std::string> labels;
  labels.reserve(tabs.size());
  for (const LeaderboardTab& tab : tabs) {
    labels.push_back(tab.label);
  }
  TabStrip strip(
      std::move(labels), open,
      {.previous = keyName(PREVIOUS_TAB_KEY), .next = keyName(NEXT_TAB_KEY)},
      LEADERBOARD_TABS_TOP_LEFT);
  strip.setWidth(LEADERBOARD_TABS_WIDTH);
  return strip;
}

/** @returns The tab an event asks for: a key of the tabs, or a left press on
 * the strip; nothing otherwise. */
std::optional<std::size_t> tabAskedBy(const sf::Event& event,
                                      const TabStrip& strip,
                                      const PlayfieldMapping& pointAt) {
  if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
    if (key->scancode == PREVIOUS_TAB_KEY) {
      return strip.previousTab();
    }
    return key->scancode == NEXT_TAB_KEY ? std::optional(strip.nextTab())
                                         : std::nullopt;
  }
  const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
  if (click == nullptr || click->button != sf::Mouse::Button::Left) {
    return std::nullopt;
  }
  return strip.tabOpenedAt(interfacePixelAt(pointAt(click->position)));
}

/** @brief Paints the banners and the guards: each is a rectangle of one sheet,
 * so the moves cost no work on their pixels.
 * @param time The time the animations are shown at; none shows their first
 * frames, each figure still. */
void paintCourt(IInterfaceCanvas& canvas,
                std::optional<engine::Duration> time) {
  for (const LeaderboardFigure& figure : LEADERBOARD_FIGURES) {
    const engine::Duration shown =
        time.has_value() ? *time + figure.lead : engine::Duration{};
    const std::optional<game::SpriteFrame> frame =
        figure.animation->frameAt(shown);
    if (frame.has_value()) {
      canvas.drawImage(TextureArea{.assetId = LEADERBOARD_SHEET_ID,
                                   .rectangle = rectangleOf(*frame)},
                       ImagePlacement{.topLeft = figure.topLeft},
                       INTERFACE_AREA);
    }
  }
}

/** @brief Paints the title and its rule, and the panel. */
void paintPanel(IInterfaceCanvas& canvas) {
  canvas.write(LEADERBOARD_LABEL, LEADERBOARD_TITLE_ANCHOR,
               LEADERBOARD_TITLE_STYLE);
  canvas.fill(LEADERBOARD_RULE, RULE_COLOR);
  CutCornerBox(LEADERBOARD_PANEL,
               {.border = PANEL_BORDER_COLOR, .inside = PANEL_INSIDE_COLOR})
      .paint(canvas);
}

/** @brief Writes that the server is being asked, a dot more every
 * ASKING_DOT_TIME. */
void paintAsking(IInterfaceCanvas& canvas, engine::Duration time) {
  const auto dots =
      static_cast<std::size_t>((time / ASKING_DOT_TIME) % ASKING_DOT_STEPS);
  canvas.write(std::string(ASKING_TEXT).append(dots, '.'), ASKING_ANCHOR,
               {.font = TextFont::Regular, .color = MESSAGE_LINE_COLOR});
}

/** @brief Writes what the panel says in place of rows. */
void paintMessage(IInterfaceCanvas& canvas, LeaderboardMessage message) {
  canvas.write(message.line, MESSAGE_LINE_ANCHOR,
               {.font = TextFont::Regular,
                .color = MESSAGE_LINE_COLOR,
                .alignment = TextAlignment::Center});
  canvas.write(message.detail, MESSAGE_DETAIL_ANCHOR,
               {.font = TextFont::Small,
                .color = MESSAGE_DETAIL_COLOR,
                .alignment = TextAlignment::Center});
}

}  // namespace

LeaderboardScreen::LeaderboardScreen(LeaderboardScreenParts parts,
                                     std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      sounds_(&parts.sounds.get()),
      tabs_(std::move(parts.tabs)),
      choice_(parts.choice),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      keyName_(std::move(parts.keyName)),
      motion_(parts.motion),
      filterDialog_(parts.sounds.get()),
      tabStrip_(tabStripOf(tabs_, 0, keyName_)),
      list_(LEADERBOARD_LIST),
      backChip_(backChipParts([this] { navigator_->pop(); }, parts.sounds)),
      filterChip_(filterChipParts([this] { openFilter(); }, parts.sounds)),
      joinChip_(joinBarParts(
          [this] {
            navigator_->pop();
            navigator_->push(CONNECTION_SCREEN);
          },
          parts.sounds)) {
  ask();
  showPage(shownPage());
  repaint();
}

LeaderboardScreen::~LeaderboardScreen() {
  for (const LeaderboardTab& tab : tabs_) {
    if (tab.source != nullptr) {
      tab.source->stopAsking();
    }
  }
}

void LeaderboardScreen::handleEvent(const sf::Event& event) {
  untilRepaint_ = engine::Duration{};
  if (filterDialog_.isOpen()) {
    filterDialog_.handleEvent(event, playfieldPointAt_);
    return;
  }
  if (backChip_.handleEvent(event, playfieldPointAt_) ||
      filterChip_.handleEvent(event, playfieldPointAt_) ||
      joinChip_.handleEvent(event, playfieldPointAt_)) {
    return;
  }
  const std::optional<std::size_t> tab =
      tabAskedBy(event, tabStrip_, playfieldPointAt_);
  if (tab.has_value() && *tab != openTab_) {
    openTab(*tab);
  }
}

void LeaderboardScreen::update(engine::Duration elapsed) {
  elapsed_ += elapsed;
  untilRepaint_ -= elapsed;
  filterDialog_.advance(elapsed);
  const LeaderboardPage& page = shownPage();
  if (shownRevision_ != page.revision) {
    showPage(page);
    untilRepaint_ = engine::Duration{};
  }
  if (untilRepaint_ <= engine::Duration{}) {
    repaint();
  }
}

void LeaderboardScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool LeaderboardScreen::hidesScreensBelow() const { return true; }

void LeaderboardScreen::ask() {
  ILeaderboardSource* source = tabs_.at(openTab_).source;
  if (source != nullptr) {
    source->ask(filterOf(choice_));
  }
}

void LeaderboardScreen::openTab(std::size_t tab) {
  sounds_->play(CLICK_SOUND_ID);
  openTab_ = tab;
  tabStrip_ = tabStripOf(tabs_, tab, keyName_);
  ask();
  showPage(shownPage());
}

void LeaderboardScreen::openFilter() {
  filterDialog_.open(choice_, [this](LeaderboardFilterChoice picked) {
    choice_ = picked;
    ask();
  });
}

const LeaderboardPage& LeaderboardScreen::shownPage() const {
  const LeaderboardTab& tab = tabs_.at(openTab_);
  return tab.source != nullptr ? tab.source->page() : pageWithoutSource_;
}

void LeaderboardScreen::showPage(const LeaderboardPage& page) {
  shownRevision_ = page.revision;
  list_.setRows(itemRowsOf(page));
  list_.setPinnedRow(pinnedRowOf(page));
  list_.setPinnedNote(pinnedNoteOf(page));
  filterChip_.setEnabled(canBeFiltered(page));
  joinChip_.setEnabled(page.phase == LeaderboardPhase::NotOnServer);
}

void LeaderboardScreen::repaint() {
  const bool moves = motion_ == MotionPreference::Full;
  const LeaderboardPage& page = shownPage();
  const std::optional<LeaderboardMessage> message = messageOf(page);
  canvas_->clear();
  paintCourt(*canvas_, moves ? std::optional(elapsed_) : std::nullopt);
  paintPanel(*canvas_);
  tabStrip_.paint(*canvas_);
  if (page.phase == LeaderboardPhase::Asking) {
    paintAsking(*canvas_, moves ? elapsed_ : engine::Duration{});
  } else if (message.has_value()) {
    paintMessage(*canvas_, *message);
  }
  list_.paint(*canvas_);
  if (page.phase == LeaderboardPhase::NotOnServer) {
    joinChip_.paint(*canvas_);
  }
  backChip_.paint(*canvas_);
  filterChip_.paint(*canvas_);
  if (canBeFiltered(page)) {
    canvas_->write(filterSummaryOf(choice_), FILTER_SUMMARY_ANCHOR,
                   FILTER_SUMMARY_STYLE);
  }
  filterDialog_.paint(*canvas_);
  canvas_->display();
  untilRepaint_ = LEADERBOARD_REPAINT_TIME;
}

}  // namespace rtype::client

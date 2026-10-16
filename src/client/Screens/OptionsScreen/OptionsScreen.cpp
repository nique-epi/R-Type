#include "OptionsScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "BoundAction.hpp"
#include "ChoiceSelector.hpp"
#include "ColorMode.hpp"
#include "Dropdown.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "ISettingsStorage.hpp"
#include "ISizableWindow.hpp"
#include "KeyBindings.hpp"
#include "KeyNames.hpp"
#include "KeySlot.hpp"
#include "LinkText.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "OptionsScreenConstants.hpp"
#include "PixelSize.hpp"
#include "PlayerSettings.hpp"
#include "PlayfieldMapping.hpp"
#include "RowControl.hpp"
#include "ScreenConstants.hpp"
#include "SettingsConstants.hpp"
#include "Slider.hpp"
#include "Switch.hpp"
#include "TimeConstants.hpp"
#include "ValueBinding.hpp"
#include "WidgetConstants.hpp"
#include "WindowConstants.hpp"
#include "WindowSizeChart.hpp"
#include "WindowSizeChartConstants.hpp"
#include "WindowSizeSelection.hpp"
#include "WindowSizeText.hpp"

namespace rtype::client {

namespace {

/** @returns The description of a volume: its step and its default, or why
 * its slider is dimmed while the sound is muted. */
std::string volumeDescription(int defaultVolume, bool muted) {
  if (muted) {
    return MUTED_VOLUME_DESCRIPTION;
  }
  std::string text(VOLUME_DESCRIPTION_STEP);
  text += std::to_string(VOLUME_STEP);
  text += VOLUME_DESCRIPTION_DEFAULT;
  text += std::to_string(defaultVolume);
  text += VOLUME_DESCRIPTION_END;
  return text;
}

/** @returns A description naming a window size, then saying why. */
std::string sizeDescription(std::size_t index, std::string_view reason) {
  std::string text(DESCRIPTION_START);
  text += windowSizeText(WINDOW_SIZES[index]);
  text += reason;
  return text;
}

/** @returns The texts of a list of labels. */
template <std::size_t Count>
std::vector<std::string> textsOf(
    const std::array<std::string_view, Count>& labels) {
  return {labels.begin(), labels.end()};
}

}  // namespace

OptionsScreen::OptionsScreen(IScreenNavigator& navigator, Place place,
                             PlayerSettings& settings,
                             ISettingsStorage& storage, ISizableWindow& window,
                             KeyNaming keyName,
                             std::unique_ptr<IInterfaceCanvas> canvas,
                             PlayfieldMapping playfieldPointAt)
    : navigator_(&navigator),
      place_(place),
      settings_(&settings),
      storage_(&storage),
      window_(&window),
      keyName_(std::move(keyName)),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(playfieldPointAt)),
      tabs_(buildTabs()) {
  refresh();
  repaint();
}

void OptionsScreen::handleEvent(const sf::Event& event) {
  if (dialog_.has_value()) {
    dialog_->handleEvent(event, playfieldPointAt_);
    closeDialogIfAnswered();
    return;
  }
  openMenu().handleEvent(event, playfieldPointAt_);
}

void OptionsScreen::update(engine::Duration elapsed) {
  if (dialog_.has_value()) {
    dialog_->advance(elapsed);
    closeDialogIfAnswered();
  } else {
    openMenu().advance(elapsed);
  }
  ageStamp(elapsed);
  save();
  refresh();
  repaint();
}

void OptionsScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool OptionsScreen::hidesScreensBelow() const {
  return place_ == Place::OverTitle;
}

std::vector<Menu> OptionsScreen::buildTabs() {
  std::vector<Menu> tabs;
  tabs.reserve(OPTIONS_TAB_COUNT);
  tabs.push_back(soundTab());
  tabs.push_back(controlsTab());
  tabs.push_back(displayTab());
  tabs.push_back(accessTab());
  return tabs;
}

MenuBuilder OptionsScreen::tabbedPanel(std::size_t tab) {
  MenuBuilder builder(MenuBuilder::Frame::TabbedPanel);
  builder.heading(SETTINGS_HEADING)
      .tabs(textsOf(OPTIONS_TAB_LABELS), tab,
            {.previous = keyName_(PREVIOUS_TAB_KEY),
             .next = keyName_(NEXT_TAB_KEY)},
            [this](std::size_t opened) { openTab(opened); });
  if (place_ == Place::OverGame) {
    builder.dimScreensBelow().notice(GAME_GOES_ON_NOTICE);
  }
  return builder;
}

Menu OptionsScreen::withBack(MenuBuilder& builder) {
  return builder
      .cancelButton({.label = BACK_LABEL, .description = BACK_DESCRIPTION},
                    [this] { close(); })
      .build();
}

Menu OptionsScreen::soundTab() {
  MenuBuilder builder = tabbedPanel(SOUND_TAB);
  builder
      .row({.label = MUSIC_LABEL}, volumeSlider(&PlayerSettings::musicVolume))
      .row({.label = EFFECTS_LABEL},
           volumeSlider(&PlayerSettings::effectsVolume))
      .row({.label = MUTE_ALL_LABEL, .description = MUTE_ALL_DESCRIPTION},
           settingSwitch(&PlayerSettings::muted))
      .row({.label = DEFAULT_SOUND_LABEL,
            .description = DEFAULT_SOUND_DESCRIPTION},
           resetLink(DEFAULT_SOUND_QUESTION, [this] { resetSound(); }));
  return withBack(builder);
}

Menu OptionsScreen::controlsTab() {
  MenuBuilder builder = tabbedPanel(CONTROLS_TAB);
  for (std::size_t row = 0; row < BOUND_ACTION_COUNT; ++row) {
    std::unique_ptr<KeySlot> slot = keySlot(BOUND_ACTIONS[row]);
    keySlots_[row] = slot.get();
    builder.row({.label = std::string(BOUND_ACTION_LABELS[row])},
                std::move(slot));
  }
  builder.row(
      {.label = DEFAULT_KEYS_LABEL, .description = DEFAULT_KEYS_DESCRIPTION},
      resetLink(DEFAULT_KEYS_QUESTION, [this] { resetKeys(); }));
  return withBack(builder);
}

Menu OptionsScreen::displayTab() {
  std::vector<std::string> sizes;
  sizes.reserve(WINDOW_SIZES.size());
  for (const PixelSize size : WINDOW_SIZES) {
    sizes.push_back(windowSizeText(size));
  }
  auto list = std::make_unique<Dropdown>(
      std::move(sizes),
      [this](std::size_t index) {
        return window_->windowSizeSelection().isAvailable(index);
      },
      ValueBinding<std::size_t>{
          .read =
              [this] { return window_->windowSizeSelection().selectedIndex(); },
          .write = [this](std::size_t index) { chooseWindowSize(index); }});
  windowSizes_ = list.get();
  MenuBuilder builder = tabbedPanel(DISPLAY_TAB);
  builder.row({.label = WINDOW_SIZE_LABEL}, std::move(list))
      .illustration(WINDOW_SIZE_CHART_SIZE);
  return withBack(builder);
}

Menu OptionsScreen::accessTab() {
  MenuBuilder builder = tabbedPanel(ACCESS_TAB);
  builder
      .row({.label = COLOR_MODE_LABEL, .description = COLOR_MODE_DESCRIPTION},
           std::make_unique<ChoiceSelector>(
               textsOf(COLOR_MODE_LABELS),
               ValueBinding<std::size_t>{
                   .read =
                       [this] {
                         return static_cast<std::size_t>(settings_->colorMode);
                       },
                   .write =
                       [this](std::size_t mode) {
                         settings_->colorMode = COLOR_MODES[mode];
                         changed();
                       }}))
      .row({.label = REDUCE_FLASHES_LABEL,
            .description = REDUCE_FLASHES_DESCRIPTION},
           settingSwitch(&PlayerSettings::reduceFlashes))
      .row({.label = SOUND_CUES_LABEL, .description = SOUND_CUES_DESCRIPTION},
           settingSwitch(&PlayerSettings::soundCues))
      .row({.label = AUTO_FIRE_LABEL, .description = AUTO_FIRE_DESCRIPTION},
           settingSwitch(&PlayerSettings::autoFire));
  return withBack(builder);
}

std::unique_ptr<RowControl> OptionsScreen::volumeSlider(
    int PlayerSettings::* volume) {
  return std::make_unique<Slider>(
      VOLUME_RANGE,
      ValueBinding<int>{.read = [this, volume] { return settings_->*volume; },
                        .write =
                            [this, volume](int value) {
                              settings_->*volume = value;
                              changed();
                            }},
      [this] { return settings_->muted; }, PERCENT_UNIT);
}

std::unique_ptr<RowControl> OptionsScreen::settingSwitch(
    bool PlayerSettings::* setting) {
  return std::make_unique<Switch>(ValueBinding<bool>{
      .read = [this, setting] { return settings_->*setting; },
      .write =
          [this, setting](bool switchedOn) {
            settings_->*setting = switchedOn;
            changed();
          }});
}

std::unique_ptr<KeySlot> OptionsScreen::keySlot(BoundAction action) {
  return std::make_unique<KeySlot>(KeySlot::Wiring{
      .key = {.read = [this, action] { return settings_->keys.keyOf(action); },
              .write =
                  [this, action](sf::Keyboard::Scancode key) {
                    settings_->keys.bind(action, key);
                    changed();
                  }},
      .name = keyName_,
      .accepts =
          [](sf::Keyboard::Scancode key) { return keyName(key).has_value(); },
      .conflicts =
          [this, action] {
            return settings_->keys.otherActionWithKeyOf(action).has_value();
          }});
}

std::unique_ptr<RowControl> OptionsScreen::resetLink(
    std::string question, std::function<void()> reset) {
  return std::make_unique<LinkText>(
      RESET_TEXT, [this, question = std::move(question),
                   reset = std::move(reset)] { askToReset(question, reset); });
}

void OptionsScreen::openTab(std::size_t tab) {
  if (tab < tabs_.size()) {
    openTab_ = tab;
  }
}

void OptionsScreen::chooseWindowSize(std::size_t index) {
  if (window_->selectWindowSize(index)) {
    settings_->windowSize = WINDOW_SIZES[index];
    changed();
  }
}

void OptionsScreen::resetSound() {
  const PlayerSettings firstLaunch;
  settings_->musicVolume = firstLaunch.musicVolume;
  settings_->effectsVolume = firstLaunch.effectsVolume;
  settings_->muted = firstLaunch.muted;
}

void OptionsScreen::resetKeys() { settings_->keys = KeyBindings(); }

void OptionsScreen::askToReset(const std::string& question,
                               const std::function<void()>& reset) {
  dialog_ =
      MenuBuilder(MenuBuilder::Frame::Dialog)
          .heading(CONFIRM_HEADING)
          .message(question)
          .cancelButton({.label = NO_LABEL}, [this] { dialogAnswered_ = true; })
          .button({.label = YES_LABEL},
                  [this, reset] {
                    reset();
                    changed();
                    dialogAnswered_ = true;
                  })
          .build();
}

void OptionsScreen::closeDialogIfAnswered() {
  if (dialogAnswered_) {
    dialogAnswered_ = false;
    dialog_.reset();
  }
}

void OptionsScreen::changed() { unsaved_ = true; }

void OptionsScreen::save() {
  if (!unsaved_) {
    return;
  }
  unsaved_ = false;
  if (storage_->save(*settings_)) {
    stamp_ = SAVED_STAMP;
    stampAge_ = engine::Duration::zero();
    return;
  }
  stamp_ = NOT_SAVED_STAMP;
  stampAge_.reset();
}

void OptionsScreen::close() {
  save();
  navigator_->pop();
}

void OptionsScreen::ageStamp(engine::Duration elapsed) {
  if (!stampAge_.has_value()) {
    return;
  }
  *stampAge_ += elapsed;
  if (*stampAge_ >= SAVED_STAMP_DURATION) {
    stamp_.clear();
    stampAge_.reset();
  }
}

void OptionsScreen::refresh() {
  Menu& sound = tabs_[SOUND_TAB];
  sound.setDescription(
      MUSIC_ROW, volumeDescription(DEFAULT_MUSIC_VOLUME, settings_->muted));
  sound.setDescription(
      EFFECTS_ROW, volumeDescription(DEFAULT_EFFECTS_VOLUME, settings_->muted));
  for (std::size_t row = 0; row < BOUND_ACTION_COUNT; ++row) {
    tabs_[CONTROLS_TAB].setDescription(FIRST_KEY_ROW + row,
                                       keyDescription(row));
  }
  tabs_[DISPLAY_TAB].setDescription(WINDOW_SIZE_ROW, windowSizeDescription());
  openMenu().setStatus(stamp_);
}

std::string OptionsScreen::keyDescription(std::size_t row) const {
  const BoundAction action = BOUND_ACTIONS[row];
  if (keySlots_[row]->isListening()) {
    return keySlots_[row]->lastKeyRefused() ? REFUSED_KEY_DESCRIPTION
                                            : LISTENING_DESCRIPTION;
  }
  const std::optional<BoundAction> other =
      settings_->keys.otherActionWithKeyOf(action);
  if (other.has_value()) {
    std::string text(CONFLICT_DESCRIPTION_START);
    text += BOUND_ACTION_LABELS[static_cast<std::size_t>(*other)];
    text += CONFLICT_DESCRIPTION_END;
    return text;
  }
  return action == BoundAction::Ready ? READY_KEY_DESCRIPTION : KEY_DESCRIPTION;
}

std::string OptionsScreen::windowSizeDescription() const {
  const WindowSizeSelection& selection = window_->windowSizeSelection();
  if (windowSizes_->isOpen()) {
    const std::optional<std::size_t> pointed = windowSizes_->pointedChoice();
    if (pointed.has_value() && !selection.isAvailable(*pointed)) {
      return sizeDescription(*pointed, POINTED_TOO_LARGE_END);
    }
    return OPEN_LIST_DESCRIPTION;
  }
  const std::size_t larger = selection.selectedIndex() + 1;
  if (larger < WINDOW_SIZES.size() && !selection.isAvailable(larger)) {
    return sizeDescription(larger, LARGER_TOO_LARGE_END);
  }
  return WINDOW_SIZE_DESCRIPTION;
}

Menu& OptionsScreen::openMenu() { return tabs_[openTab_]; }

const Menu& OptionsScreen::openMenu() const { return tabs_[openTab_]; }

void OptionsScreen::repaint() {
  canvas_->clear();
  const Menu& menu = openMenu();
  menu.paint(*canvas_);
  const std::optional<sf::IntRect> chart = menu.illustrationArea();
  if (chart.has_value()) {
    paintWindowSizeChart(*canvas_, chart->position,
                         window_->windowSizeSelection(),
                         windowSizes_->highlighted());
  }
  if (dialog_.has_value()) {
    dialog_->paint(*canvas_);
  }
  canvas_->display();
}

}  // namespace rtype::client

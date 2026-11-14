#include "OptionsTabs.hpp"
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "BoundAction.hpp"
#include "Dropdown.hpp"
#include "ISizableWindow.hpp"
#include "KeySlot.hpp"
#include "LinkText.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "OptionsDescriptions.hpp"
#include "OptionsPlace.hpp"
#include "OptionsRowHandles.hpp"
#include "OptionsRows.hpp"
#include "OptionsScreenConstants.hpp"
#include "OptionsTabsParts.hpp"
#include "PlayerSettings.hpp"
#include "RowControl.hpp"
#include "ScreenConstants.hpp"
#include "SettingDefinitionConstants.hpp"
#include "SettingsConstants.hpp"
#include "SettingsService.hpp"
#include "TextMeasures.hpp"
#include "TextSize.hpp"
#include "WidgetConstants.hpp"
#include "WindowLayout.hpp"
#include "WindowSizeChartConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The builder of the panel of the tab @p tab: its heading and its
 * tab strip, and the notice that the game goes on when the options are over
 * a game. */
MenuBuilder tabbedPanel(const OptionsTabsParts& parts, std::size_t tab) {
  MenuBuilder builder(MenuBuilder::Frame::TabbedPanel, &parts.sounds.get(),
                      textMetricsOf(parts.service.get().settings().textSize));
  builder.heading(SETTINGS_HEADING)
      .tabs(textsOf(OPTIONS_TAB_LABELS), tab,
            {.previous = parts.keyName(PREVIOUS_TAB_KEY),
             .next = parts.keyName(NEXT_TAB_KEY)},
            parts.openTab);
  if (parts.place == OptionsPlace::OverGame) {
    builder.dimScreensBelow().notice(GAME_GOES_ON_NOTICE);
  }
  return builder;
}

/** @returns The menu of @p builder with BACK under its rows and, over a game,
 * QUIT at the right end of the row of BACK. */
Menu withBack(const OptionsTabsParts& parts, MenuBuilder& builder) {
  builder.cancelButton({.label = BACK_LABEL, .description = BACK_DESCRIPTION},
                       parts.close);
  if (parts.place == OptionsPlace::OverGame) {
    builder.button({.label = QUIT_LABEL, .description = QUIT_GAME_DESCRIPTION},
                   parts.leaveGame);
  }
  return builder.build();
}

/** @returns The link that asks @p question and runs @p reset on YES. */
std::unique_ptr<RowControl> resetLink(const OptionsTabsParts& parts,
                                      std::string question,
                                      std::function<void()> reset) {
  return std::make_unique<LinkText>(
      RESET_TEXT, [ask = parts.askToReset, question = std::move(question),
                   reset = std::move(reset)] { ask(question, reset); });
}

Menu soundTab(const OptionsTabsParts& parts) {
  SettingsService& service = parts.service.get();
  MenuBuilder builder = tabbedPanel(parts, SOUND_TAB);
  builder
      .row({.label = MUSIC_LABEL}, volumeSlider(service, MUSIC_VOLUME_SETTING))
      .row({.label = EFFECTS_LABEL},
           volumeSlider(service, EFFECTS_VOLUME_SETTING))
      .row({.label = MUTE_ALL_LABEL, .description = MUTE_ALL_DESCRIPTION},
           settingSwitch(service, MUTED_SETTING))
      .row({.label = DEFAULT_SOUND_LABEL,
            .description = DEFAULT_SOUND_DESCRIPTION},
           resetLink(parts, DEFAULT_SOUND_QUESTION,
                     [&service] { service.resetSound(); }));
  return withBack(parts, builder);
}

Menu controlsTab(const OptionsTabsParts& parts, OptionsRowHandles& handles) {
  SettingsService& service = parts.service.get();
  MenuBuilder builder = tabbedPanel(parts, CONTROLS_TAB);
  for (std::size_t row = 0; row < OPEN_CHAT_ACTION; ++row) {
    std::unique_ptr<KeySlot> slot =
        keySlot(service, parts.keyName, BOUND_ACTIONS[row]);
    handles.keySlots[row] = slot.get();
    builder.row({.label = std::string(BOUND_ACTION_LABELS[row])},
                std::move(slot));
  }
  builder.row(
      {.label = DEFAULT_KEYS_LABEL, .description = DEFAULT_KEYS_DESCRIPTION},
      resetLink(parts, DEFAULT_KEYS_QUESTION,
                [&service] { service.resetKeys(); }));
  return withBack(parts, builder);
}

Menu displayTab(const OptionsTabsParts& parts, OptionsRowHandles& handles) {
  std::unique_ptr<Dropdown> modes =
      displayModeList(parts.window.get(), parts.service.get());
  handles.displayModes = modes.get();
  std::unique_ptr<Dropdown> sizes =
      windowSizeList(parts.window.get(), parts.service.get());
  handles.windowSizes = sizes.get();
  MenuBuilder builder = tabbedPanel(parts, DISPLAY_TAB);
  builder.row({.label = DISPLAY_MODE_LABEL}, std::move(modes))
      .row({.label = WINDOW_SIZE_LABEL}, std::move(sizes))
      .illustration(WINDOW_SIZE_CHART_SIZE);
  return withBack(parts, builder);
}

Menu accessTab(const OptionsTabsParts& parts) {
  SettingsService& service = parts.service.get();
  MenuBuilder builder = tabbedPanel(parts, ACCESS_TAB);
  builder
      .row({.label = TEXT_SIZE_LABEL, .description = TEXT_SIZE_DESCRIPTION},
           textSizeSelector(service))
      .row({.label = HIGH_CONTRAST_LABEL,
            .description = HIGH_CONTRAST_DESCRIPTION},
           settingSwitch(service, HIGH_CONTRAST_SETTING))
      .row({.label = COLOR_MODE_LABEL, .description = COLOR_MODE_DESCRIPTION},
           colorModeSelector(service))
      .row({.label = REDUCE_FLASHES_LABEL,
            .description = REDUCE_FLASHES_DESCRIPTION},
           settingSwitch(service, REDUCE_FLASHES_SETTING))
      .row({.label = SOUND_CUES_LABEL, .description = SOUND_CUES_DESCRIPTION},
           settingSwitch(service, SOUND_CUES_SETTING))
      .row({.label = AUTO_FIRE_LABEL, .description = AUTO_FIRE_DESCRIPTION},
           settingSwitch(service, AUTO_FIRE_SETTING))
      .row({.label = PILOT_NAMES_LABEL, .description = PILOT_NAMES_DESCRIPTION},
           settingSwitch(service, PILOT_NAMES_SETTING))
      .row(
          {.label = LAGOMETER_SCALE_LABEL,
           .description = LAGOMETER_SCALE_DESCRIPTION},
          settingSlider(service, LAGOMETER_SCALE_SETTING, LAGOMETER_SCALE_LOOK))
      .row({.label = LAGOMETER_BACKGROUND_LABEL,
            .description = LAGOMETER_BACKGROUND_DESCRIPTION},
           settingSlider(service, LAGOMETER_BACKGROUND_SETTING,
                         LAGOMETER_BACKGROUND_LOOK));
  return withBack(parts, builder);
}

Menu chatTab(const OptionsTabsParts& parts, OptionsRowHandles& handles) {
  SettingsService& service = parts.service.get();
  std::unique_ptr<KeySlot> slot =
      keySlot(service, parts.keyName, BoundAction::OpenChat);
  handles.keySlots[OPEN_CHAT_ACTION] = slot.get();
  MenuBuilder builder = tabbedPanel(parts, CHAT_TAB);
  builder
      .row({.label = CHAT_SHOWN_LABEL, .description = CHAT_SHOWN_DESCRIPTION},
           settingSwitch(service, CHAT_SHOWN_SETTING))
      .row({.label = std::string(BOUND_ACTION_LABELS[OPEN_CHAT_ACTION])},
           std::move(slot))
      .row({.label = CHAT_LETTERS_LABEL,
            .description = CHAT_LETTERS_DESCRIPTION},
           chatSlider(service, CHAT_LETTERS_SETTING, CHAT_LETTERS_LOOK))
      .row({.label = CHAT_CLOSED_LINES_LABEL,
            .description = CHAT_CLOSED_LINES_DESCRIPTION},
           chatSlider(service, CHAT_CLOSED_LINES_SETTING, CHAT_LINES_LOOK))
      .row({.label = CHAT_OPEN_LINES_LABEL,
            .description = CHAT_OPEN_LINES_DESCRIPTION},
           chatSlider(service, CHAT_OPEN_LINES_SETTING, CHAT_LINES_LOOK))
      .row({.label = CHAT_SECONDS_LABEL,
            .description = CHAT_SECONDS_DESCRIPTION},
           chatSlider(service, CHAT_SECONDS_SETTING, CHAT_SECONDS_LOOK))
      .row({.label = CHAT_BACKGROUND_LABEL,
            .description = CHAT_BACKGROUND_DESCRIPTION},
           chatSlider(service, CHAT_BACKGROUND_SETTING, CHAT_BACKGROUND_LOOK));
  return withBack(parts, builder);
}

/** @returns The tabs, in the order of SOUND_TAB, CONTROLS_TAB, DISPLAY_TAB,
 * ACCESS_TAB and CHAT_TAB, and fills @p handles with the rows whose state the
 * descriptions follow. */
std::vector<Menu> buildTabs(const OptionsTabsParts& parts,
                            OptionsRowHandles& handles) {
  std::vector<Menu> tabs;
  tabs.reserve(OPTIONS_TAB_COUNT);
  tabs.push_back(soundTab(parts));
  tabs.push_back(controlsTab(parts, handles));
  tabs.push_back(displayTab(parts, handles));
  tabs.push_back(accessTab(parts));
  tabs.push_back(chatTab(parts, handles));
  return tabs;
}

}  // namespace

OptionsTabs::OptionsTabs(const OptionsTabsParts& parts)
    : parts_(parts),
      service_(&parts.service.get()),
      window_(&parts.window.get()),
      builtSize_(service_->settings().textSize),
      tabs_(buildTabs(parts, handles_)) {}

void OptionsTabs::followTextSize() {
  const TextSize size = service_->settings().textSize;
  if (size == builtSize_) {
    return;
  }
  builtSize_ = size;
  tabs_ = buildTabs(parts_, handles_);
}

void OptionsTabs::openTab(std::size_t tab) {
  if (tab < tabs_.size()) {
    openTab_ = tab;
  }
}

Menu& OptionsTabs::openMenu() { return tabs_[openTab_]; }

const Menu& OptionsTabs::openMenu() const { return tabs_[openTab_]; }

const Dropdown& OptionsTabs::windowSizes() const {
  return *handles_.windowSizes;
}

void OptionsTabs::refreshDescriptions() {
  const PlayerSettings& settings = service_->settings();
  Menu& sound = tabs_[SOUND_TAB];
  sound.setDescription(MUSIC_ROW,
                       volumeDescription(DEFAULT_MUSIC_VOLUME, settings.muted));
  sound.setDescription(
      EFFECTS_ROW, volumeDescription(DEFAULT_EFFECTS_VOLUME, settings.muted));
  for (std::size_t row = 0; row < BOUND_ACTION_COUNT; ++row) {
    const bool chatKey = row == OPEN_CHAT_ACTION;
    tabs_[chatKey ? CHAT_TAB : CONTROLS_TAB].setDescription(
        chatKey ? CHAT_KEY_ROW : FIRST_KEY_ROW + row,
        keyDescription(*handles_.keySlots[row],
                       settings.keys.otherActionWithKeyOf(BOUND_ACTIONS[row])));
  }
  const WindowLayout& layout = window_->windowLayout();
  Menu& display = tabs_[DISPLAY_TAB];
  display.setDescription(
      DISPLAY_MODE_ROW,
      displayModeDescription(*handles_.displayModes, layout.displayMode()));
  display.setEnabled(WINDOW_SIZE_ROW, !layout.imposedSize().has_value());
  display.setDescription(
      WINDOW_SIZE_ROW,
      windowSizeDescription(*handles_.windowSizes, layout.sizes()));
}

}  // namespace rtype::client

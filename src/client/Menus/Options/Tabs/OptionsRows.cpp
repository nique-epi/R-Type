#include "OptionsRows.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "BoundAction.hpp"
#include "ChoiceList.hpp"
#include "ChoiceSelector.hpp"
#include "ColorMode.hpp"
#include "Dropdown.hpp"
#include "DropdownParts.hpp"
#include "ISizableWindow.hpp"
#include "KeyNames.hpp"
#include "KeySlot.hpp"
#include "OptionsScreenConstants.hpp"
#include "PixelSize.hpp"
#include "RowControl.hpp"
#include "Setting.hpp"
#include "SettingDefinitionConstants.hpp"
#include "SettingRange.hpp"
#include "SettingsConstants.hpp"
#include "SettingsService.hpp"
#include "Slider.hpp"
#include "SliderLook.hpp"
#include "Switch.hpp"
#include "TextSize.hpp"
#include "ValueBinding.hpp"
#include "WindowConstants.hpp"
#include "WindowLayout.hpp"
#include "WindowSizeText.hpp"

namespace rtype::client {

namespace {

/** @returns A row that cycles through @p choices, showing the value of
 * @p setting and changing it through @p service. */
template <typename Value>
std::unique_ptr<RowControl> choiceSelectorOf(SettingsService& service,
                                             const Setting<Value>& setting,
                                             ChoiceList<Value> choices) {
  return std::make_unique<ChoiceSelector>(
      std::move(choices.texts),
      ValueBinding<std::size_t>{
          .read =
              [&service, setting, values = choices.values] {
                const Value current = service.settings().*setting.member;
                return static_cast<std::size_t>(
                    std::ranges::find(values, current) - values.begin());
              },
          .write =
              [&service, setting, values = choices.values](std::size_t index) {
                service.apply(setting, values[index]);
              }});
}

/** @returns The texts of the sizes of WINDOW_SIZES, in their order. */
std::vector<std::string> windowSizeTexts() {
  std::vector<std::string> texts;
  texts.reserve(WINDOW_SIZES.size());
  for (const PixelSize size : WINDOW_SIZES) {
    texts.push_back(windowSizeText(size));
  }
  return texts;
}

/** @returns The size the display mode imposes on the window, as the list of
 * sizes writes it, or nothing while the player picks the size. */
std::optional<std::string> imposedSizeText(const WindowLayout& layout) {
  const std::optional<PixelSize> imposed = layout.imposedSize();
  if (!imposed.has_value()) {
    return std::nullopt;
  }
  return windowSizeText(*imposed);
}

/** @returns What reads @p setting through @p service and changes it
 * there. */
template <typename Value>
ValueBinding<Value> bindingOf(SettingsService& service,
                              const Setting<Value>& setting) {
  return ValueBinding<Value>{
      .read = [&service,
               &setting] { return service.settings().*setting.member; },
      .write = [&service,
                &setting](Value value) { service.apply(setting, value); }};
}

/** @returns The range a slider of @p setting moves over, by the step of
 * @p look. */
Slider::Range sliderRangeOf(const Setting<int>& setting,
                            const SliderLook& look) {
  const SettingRange range = setting.range.value_or(SettingRange{});
  return Slider::Range{.minimum = range.minimum,
                       .maximum = range.maximum,
                       .step = look.step,
                       .pageSteps = CHAT_PAGE_STEPS};
}

}  // namespace

std::unique_ptr<RowControl> volumeSlider(SettingsService& service,
                                         const Setting<int>& volume) {
  return std::make_unique<Slider>(
      VOLUME_RANGE, bindingOf(service, volume),
      [&service] { return service.settings().muted; }, PERCENT_UNIT);
}

std::unique_ptr<RowControl> settingSwitch(SettingsService& service,
                                          const Setting<bool>& setting) {
  return std::make_unique<Switch>(bindingOf(service, setting));
}

std::unique_ptr<RowControl> chatSlider(SettingsService& service,
                                       const Setting<int>& setting,
                                       const SliderLook& look) {
  return std::make_unique<Slider>(
      sliderRangeOf(setting, look), bindingOf(service, setting),
      [&service] { return !service.settings().chatShown; }, look.unit);
}

std::unique_ptr<RowControl> settingSlider(SettingsService& service,
                                          const Setting<int>& setting,
                                          const SliderLook& look) {
  return std::make_unique<Slider>(
      sliderRangeOf(setting, look), bindingOf(service, setting),
      [] { return false; }, look.unit);
}

std::unique_ptr<RowControl> colorModeSelector(SettingsService& service) {
  return choiceSelectorOf(
      service, COLOR_MODE_SETTING,
      ChoiceList<ColorMode>{.values = COLOR_MODES,
                            .texts = textsOf(COLOR_MODE_LABELS)});
}

std::unique_ptr<RowControl> textSizeSelector(SettingsService& service) {
  return choiceSelectorOf(
      service, TEXT_SIZE_SETTING,
      ChoiceList<TextSize>{.values = TEXT_SIZES,
                           .texts = textsOf(TEXT_SIZE_LABELS)});
}

std::unique_ptr<KeySlot> keySlot(SettingsService& service,
                                 const KeyNaming& naming, BoundAction action) {
  return std::make_unique<KeySlot>(KeySlot::Wiring{
      .key = {.read =
                  [&service, action] {
                    return service.settings().keys.keyOf(action);
                  },
              .write =
                  [&service, action](sf::Keyboard::Scancode key) {
                    service.bindKey(action, key);
                  }},
      .name = naming,
      .accepts =
          [](sf::Keyboard::Scancode key) { return keyName(key).has_value(); },
      .conflicts =
          [&service, action] {
            return service.settings()
                .keys.otherActionWithKeyOf(action)
                .has_value();
          }});
}

std::unique_ptr<Dropdown> displayModeList(ISizableWindow& window,
                                          SettingsService& service) {
  return std::make_unique<Dropdown>(DropdownParts{
      .choices = textsOf(DISPLAY_MODE_LABELS),
      .available = [](std::size_t) { return true; },
      .value = ValueBinding<std::size_t>{
          .read =
              [&window] {
                return static_cast<std::size_t>(
                    window.windowLayout().displayMode());
              },
          .write =
              [&window, &service](std::size_t mode) {
                if (window.selectDisplayMode(DISPLAY_MODES[mode])) {
                  service.apply(DISPLAY_MODE_SETTING, DISPLAY_MODES[mode]);
                }
              }},
      .imposedText = {}});
}

std::unique_ptr<Dropdown> windowSizeList(ISizableWindow& window,
                                         SettingsService& service) {
  return std::make_unique<Dropdown>(DropdownParts{
      .choices = windowSizeTexts(),
      .available =
          [&window](std::size_t index) {
            return window.windowLayout().sizes().isAvailable(index);
          },
      .value =
          ValueBinding<std::size_t>{
              .read =
                  [&window] {
                    return window.windowLayout().sizes().selectedIndex();
                  },
              .write =
                  [&window, &service](std::size_t index) {
                    if (window.selectWindowSize(index)) {
                      service.apply(WINDOW_SIZE_SETTING,
                                    std::optional(WINDOW_SIZES[index]));
                    }
                  }},
      .imposedText =
          [&window] { return imposedSizeText(window.windowLayout()); }});
}

}  // namespace rtype::client

#include "SettingsService.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <type_traits>
#include <vector>
#include "BoundAction.hpp"
#include "Endpoint.hpp"
#include "ISettingsListener.hpp"
#include "ISettingsStorage.hpp"
#include "KeyBindings.hpp"
#include "PlayerSettings.hpp"
#include "Setting.hpp"
#include "SettingDefinitionConstants.hpp"
#include "SettingId.hpp"
#include "SettingRange.hpp"

namespace rtype::client {

namespace {

/** @returns Whether @p value is inside @p range, both ends included; always
 * true without a range. */
bool isInRange(const std::optional<SettingRange>& range, int value) {
  return !range.has_value() ||
         (value >= range->minimum && value <= range->maximum);
}

/** @returns Whether @p setting accepts @p value: only an int has a range. */
template <typename Value>
bool accepts(const Setting<Value>& setting, Value value) {
  if constexpr (std::is_same_v<Value, int>) {
    return isInRange(setting.range, value);
  } else {
    return true;
  }
}

/** @brief Tells each of @p listeners that @p setting holds its value. */
void tellListeners(const std::vector<ISettingsListener*>& listeners,
                   SettingId setting) {
  for (ISettingsListener* listener : listeners) {
    listener->settingChanged(setting);
  }
}

/** @brief Tells each of @p listeners about every setting. */
void tellEverySetting(const std::vector<ISettingsListener*>& listeners) {
  for (const SettingId setting : ALL_SETTING_IDS) {
    tellListeners(listeners, setting);
  }
}

}  // namespace

SettingsService::SettingsService(PlayerSettings& settings,
                                 ISettingsStorage& storage)
    : settings_(&settings), storage_(&storage) {}

const PlayerSettings& SettingsService::settings() const { return *settings_; }

void SettingsService::subscribe(ISettingsListener& listener) {
  listeners_.push_back(&listener);
}

void SettingsService::unsubscribe(ISettingsListener& listener) {
  std::erase(listeners_, &listener);
}

void SettingsService::restore(const PlayerSettings& kept) {
  *settings_ = kept;
  tellEverySetting(listeners_);
}

template <typename Value>
bool SettingsService::apply(const Setting<Value>& setting, Value value) {
  if (!accepts(setting, value)) {
    return false;
  }
  settings_->*setting.member = value;
  changed(setting.id);
  return true;
}

template bool SettingsService::apply<int>(const Setting<int>&, int);
template bool SettingsService::apply<bool>(const Setting<bool>&, bool);
template bool SettingsService::apply<ColorMode>(const Setting<ColorMode>&,
                                                ColorMode);
template bool SettingsService::apply<DisplayMode>(const Setting<DisplayMode>&,
                                                  DisplayMode);
template bool SettingsService::apply<std::optional<PixelSize>>(
    const Setting<std::optional<PixelSize>>&, std::optional<PixelSize>);
template bool SettingsService::apply<std::optional<network::Endpoint>>(
    const Setting<std::optional<network::Endpoint>>&,
    std::optional<network::Endpoint>);

void SettingsService::bindKey(BoundAction action, sf::Keyboard::Scancode key) {
  settings_->keys.bind(action, key);
  changed(SettingId::Keys);
}

void SettingsService::resetSound() {
  const PlayerSettings firstLaunch;
  settings_->musicVolume = firstLaunch.musicVolume;
  settings_->effectsVolume = firstLaunch.effectsVolume;
  settings_->muted = firstLaunch.muted;
  changed(SettingId::MusicVolume);
  changed(SettingId::EffectsVolume);
  changed(SettingId::Muted);
}

void SettingsService::resetKeys() {
  settings_->keys = KeyBindings();
  changed(SettingId::Keys);
}

std::optional<bool> SettingsService::saveIfChanged() {
  if (!unsaved_) {
    return std::nullopt;
  }
  unsaved_ = false;
  return storage_->save(*settings_);
}

void SettingsService::changed(SettingId changedSetting) {
  unsaved_ = true;
  tellListeners(listeners_, changedSetting);
}

}  // namespace rtype::client

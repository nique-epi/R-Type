#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <vector>
#include "BoundAction.hpp"
#include "ISettingsListener.hpp"
#include "ISettingsStorage.hpp"
#include "PlayerSettings.hpp"
#include "Setting.hpp"
#include "SettingId.hpp"

namespace rtype::client {

/**
 * @brief The one way to change the settings of the player: it applies a
 * value, remembers that it is not saved yet, and tells the listeners.
 *
 * The options, and any other module that changes a setting, go through it, so
 * the volume of the sounds follows by notification instead of by reading the
 * settings again. The settings and the storage must outlive the service, and
 * so must the listeners subscribed to it, which it never unsubscribes.
 */
class SettingsService {
 public:
  SettingsService(PlayerSettings& settings, ISettingsStorage& storage);

  [[nodiscard]] const PlayerSettings& settings() const;

  void subscribe(ISettingsListener& listener);

  /** @brief Stops telling @p listener; does nothing for a listener not
   * subscribed. */
  void unsubscribe(ISettingsListener& listener);

  /**
   * @brief Gives @p setting the value @p value and tells the listeners.
   * @returns Whether it was applied; a value out of the range of the setting,
   * ends included, changes nothing and tells nobody.
   */
  template <typename Value>
  bool apply(const Setting<Value>& setting, Value value);

  void bindKey(BoundAction action, sf::Keyboard::Scancode key);

  /** @brief Puts the volumes and MUTE ALL back to those of a first launch. */
  void resetSound();

  void resetKeys();

  /**
   * @brief Saves the settings when a change is not saved yet. The change
   * counts as saved even when the storage refuses it: the next save waits for
   * the next change.
   * @returns Nothing when there was nothing to save, else whether the storage
   * kept the settings.
   */
  [[nodiscard]] std::optional<bool> saveIfChanged();

 private:
  void changed(SettingId changedSetting);

  PlayerSettings* settings_;
  ISettingsStorage* storage_;
  std::vector<ISettingsListener*> listeners_;
  bool unsaved_ = false;
};

}  // namespace rtype::client

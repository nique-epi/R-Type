#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "HangarScreen.hpp"
#include "MotionPreference.hpp"
#include "PlayerSettings.hpp"
#include "RecordingAudioService.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSettingsStorage.hpp"
#include "RecordingSoundPlayer.hpp"
#include "SettingsService.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A hangar over settings saved to a recording storage, a recording
 * navigator and a recording canvas, the ship of the first color. */
class HangarScreenFixture : public ::testing::Test {
 protected:
  HangarScreenFixture();
  explicit HangarScreenFixture(MotionPreference motion);

  /** @brief Presses then releases @p key, then runs a frame. */
  void press(sf::Keyboard::Key key);
  /** @brief Presses @p key without releasing it, then runs a frame. */
  void hold(sf::Keyboard::Key key);
  /** @brief Presses the left button over @p pixel of the interface, then
   * runs a frame. */
  void clickAt(sf::Vector2i pixel);
  void update(engine::Duration elapsed);
  /** @brief Puts every item of the catalog in the locker of the player. */
  void holdEveryItem();

  [[nodiscard]] const PlayerSettings& settings() const;
  [[nodiscard]] const RecordingSettingsStorage& storage() const;
  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;
  [[nodiscard]] const RecordingAudioService& audio() const;
  [[nodiscard]] const InterfaceCanvasRecord& painting() const;

 private:
  PlayerSettings settings_;
  RecordingSettingsStorage storage_;
  SettingsService service_;
  RecordingNavigator navigator_;
  RecordingSoundPlayer sounds_;
  RecordingAudioService audio_{sounds_};
  InterfaceCanvasRecord painting_;
  std::unique_ptr<HangarScreen> screen_;
};

}  // namespace rtype::client::testing

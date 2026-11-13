#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "HangarScreen.hpp"
#include "PlayerSettings.hpp"
#include "RecordingAudioService.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSettingsStorage.hpp"
#include "RecordingSoundPlayer.hpp"
#include "SettingsService.hpp"

namespace rtype::client::testing {

/** @brief A hangar with full motion over settings a test may fill, driven by
 * the keys, the wheel and the pointer. */
class HangarScrollFixture : public ::testing::Test {
 protected:
  HangarScrollFixture();

  /** @brief Presses then releases @p key @p times times, a frame after each. */
  void press(sf::Keyboard::Key key, int times);
  /** @brief Turns the wheel one notch down over @p pixel of the interface,
   * then runs a frame. */
  void wheelDownOver(sf::Vector2i pixel);
  /** @brief Presses the left button over @p pixel of the interface, then
   * runs a frame. */
  void clickAt(sf::Vector2i pixel);
  /** @brief Puts every item of the catalog in the locker of the player. */
  void holdEveryItem();

  [[nodiscard]] const PlayerSettings& settings() const;
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;
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

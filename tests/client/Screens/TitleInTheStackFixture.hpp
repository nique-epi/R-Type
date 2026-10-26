#pragma once

#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <vector>
#include "IScreen.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingScreen.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"

/**
 * @brief The title screen alone in a stack, with the screens it opens to put
 * over it: the real quit confirmation, and a recording screen that hides the
 * screens below for the options, which OptionsScreenTest covers. The title and
 * the dialog ask their own recording navigators, so what they ask for is
 * written down and never applied.
 */
class TitleInTheStackFixture : public ::testing::Test {
 protected:
  TitleInTheStackFixture();

  [[nodiscard]] const RecordingNavigator& titleNavigator() const;
  [[nodiscard]] const InterfaceCanvasRecord& titleCanvas() const;
  rtype::client::ScreenStack& screens();

  /** @brief Puts a screen over the title. */
  void open(rtype::client::ScreenId screen);

  /** @brief Closes the screen on top. */
  void closeTheScreenOnTop();

  /** @brief Presses and releases the left button at a pixel of the window,
   * then lets the press last. */
  void click(sf::Vector2i position);

 private:
  std::unique_ptr<rtype::client::IScreen> build(rtype::client::ScreenId screen);

  RecordingNavigator titleNavigator_;
  InterfaceCanvasRecord titleCanvas_;
  RecordingNavigator dialogNavigator_;
  InterfaceCanvasRecord dialogCanvas_;
  std::vector<JournalEntry> journal_;
  rtype::client::ScreenStack screens_;
};

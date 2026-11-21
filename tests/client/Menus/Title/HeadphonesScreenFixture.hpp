#pragma once

#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <memory>
#include "HeadphonesScreen.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"

/** @brief HEADPHONES RECOMMENDED alone, built in SetUp() with INTRO_SCREEN
 * after it, its navigator and canvas recording. */
class HeadphonesScreenFixture : public ::testing::Test {
 protected:
  void SetUp() override;

  rtype::client::HeadphonesScreen& screen();
  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const InterfaceCanvasRecord& record() const;

  /** @returns The request the screen makes when it leaves:
   * replaceAll(INTRO_SCREEN). */
  [[nodiscard]] static NavigatorCall introAsked();

  /** @returns The smallest rectangle around the pixels painted one by one
   * since the last clear(): the plotted logo. */
  [[nodiscard]] sf::IntRect plottedBounds() const;

 private:
  RecordingNavigator navigator_;
  InterfaceCanvasRecord canvas_;
  std::unique_ptr<rtype::client::HeadphonesScreen> screen_;
};

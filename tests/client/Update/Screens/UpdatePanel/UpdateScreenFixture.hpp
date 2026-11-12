#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <memory>
#include <string_view>
#include "FakeUpdateRun.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "UpdateScreen.hpp"

/** @brief An update panel over a fake update, with a recording navigator and
 * canvas. */
class UpdateScreenFixture : public ::testing::Test {
 protected:
  UpdateScreenFixture();

  /** @brief Updates the panel frame by frame, UPDATE_FRAME each, for at least
   * @p total. */
  void runFor(std::chrono::milliseconds total);

  /** @returns Whether the panel wrote @p text since its last repaint. */
  [[nodiscard]] bool wrote(std::string_view text) const;

  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] FakeUpdateRun& run();
  [[nodiscard]] rtype::client::UpdateScreen& screen();

 private:
  RecordingNavigator navigator_;
  FakeUpdateRun run_;
  InterfaceCanvasRecord canvas_;
  std::unique_ptr<rtype::client::UpdateScreen> screen_;
};

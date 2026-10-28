#pragma once

#include <gtest/gtest.h>
#include <memory>
#include "ConnectionPhase.hpp"
#include "LostScreen.hpp"
#include "MotionPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"

/** @brief A NO SIGNAL screen alone, for a silent server with full motion,
 * over a recording navigator, canvas and sound player. */
class LostScreenFixture : public ::testing::Test {
 protected:
  LostScreenFixture();

  /** @brief Builds the screen again for a session that ended as @p ending,
   * with @p motion. */
  void rebuildFor(rtype::client::ConnectionPhase ending,
                  rtype::client::MotionPreference motion);

  [[nodiscard]] rtype::client::LostScreen& screen();
  [[nodiscard]] const RecordingNavigator& navigator() const;
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const;
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;

 private:
  RecordingNavigator navigator_;
  RecordingSoundPlayer sounds_;
  InterfaceCanvasRecord canvas_;
  std::unique_ptr<rtype::client::LostScreen> screen_;
};

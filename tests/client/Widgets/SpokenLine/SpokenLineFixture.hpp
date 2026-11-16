#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>
#include "MotionPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingSoundPlayer.hpp"
#include "SpokenLine.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A spoken line of TEST_VOICE, TEST_BUBBLE_WIDTH wide with a tail at
 * TEST_TAIL, written letter by letter, on a sound player and a canvas that
 * record what they are asked. */
class SpokenLineFixture : public ::testing::Test {
 protected:
  SpokenLineFixture();

  /** @brief Builds the line again, for a player who asked for less motion,
   * or with no voice. */
  void rebuildWithLessMotion();
  void rebuildWithoutVoice();

  [[nodiscard]] SpokenLine& line();

  /** @brief Runs frames of SHORT_FRAME until @p elapsed passed. */
  void walk(engine::Duration elapsed);

  /** @returns The texts the line writes when painted now, in order. */
  [[nodiscard]] std::vector<std::string> written();

  /** @returns What the line painted last. */
  [[nodiscard]] const InterfaceCanvasRecord& painting() const;

  /** @returns The ids of the sounds played, the first played first. */
  [[nodiscard]] const std::vector<std::string>& played() const;

 private:
  void build(MotionPreference motion, bool voiced);

  RecordingSoundPlayer sounds_;
  InterfaceCanvasRecord painting_;
  std::unique_ptr<SpokenLine> line_;
};

}  // namespace rtype::client::testing

#pragma once

#include <gtest/gtest.h>
#include "AnnouncedStart.hpp"
#include "RecordingAudioService.hpp"
#include "RecordingSky.hpp"
#include "RecordingSoundPlayer.hpp"

namespace rtype::client::testing {

/** @brief The start a waiting room announces, a recording sky whose caves
 * name CAVES_MUSIC, and a recording audio service, for openTheLevel(). */
class OpeningSkyFixture : public ::testing::Test {
 protected:
  OpeningSkyFixture();

  /** @brief Announces a level opening in OPENING_BIOME. */
  void announceTheCaves();

  /** @brief Opens the level as a game screen does before it loads. */
  void openTheLevel();

  [[nodiscard]] const RecordingSky& sky() const;
  [[nodiscard]] const RecordingAudioService& audio() const;

 private:
  AnnouncedStart start_;
  RecordingSky sky_;
  RecordingSoundPlayer sounds_;
  RecordingAudioService audio_{sounds_};
};

}  // namespace rtype::client::testing

#pragma once

#include <gtest/gtest.h>
#include "BossPresentation.hpp"
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "EntityAppearance.hpp"
#include "EntityRegistry.hpp"
#include "RecordingAudioService.hpp"
#include "RecordingSoundPlayer.hpp"

/**
 * @brief The presentation of a boss over registries of its own, playing its
 * sounds and its music on recording doubles.
 */
class BossPresentationFixture : public ::testing::Test {
 protected:
  BossPresentationFixture();

  rtype::client::BossPresentation& presentation();

  /** @brief Places the boss PRESENTED_BOSS_ID, drawn from a sheet that names
   * it PRESENTED_BOSS_NAME. */
  void placeNamedBoss();
  /** @brief Has the level name LEVEL_MUSIC as the music it plays. */
  void playLevelMusic();
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;
  [[nodiscard]] const RecordingAudioService& audio() const;

 private:
  rtype::engine::EntityRegistry entities_;
  rtype::engine::ComponentRegistry components_{entities_};
  rtype::client::EffectSpawner effects_;
  RecordingSoundPlayer sounds_;
  RecordingAudioService audio_{sounds_};
  rtype::client::BossPresentation presentation_;
  rtype::client::EntityAppearance namedBossAppearance_;
};

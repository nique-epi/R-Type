#pragma once

#include <gtest/gtest.h>
#include "BossPresentation.hpp"
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
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
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;
  [[nodiscard]] const RecordingAudioService& audio() const;

 private:
  rtype::engine::EntityRegistry entities_;
  rtype::engine::ComponentRegistry components_{entities_};
  rtype::client::EffectSpawner effects_;
  RecordingSoundPlayer sounds_;
  RecordingAudioService audio_{sounds_};
  rtype::client::BossPresentation presentation_;
};

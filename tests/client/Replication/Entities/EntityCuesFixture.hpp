#pragma once

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "Entity.hpp"
#include "EntityAppearance.hpp"
#include "EntityRegistry.hpp"
#include "RecordingSoundPlayer.hpp"

/**
 * @brief A Bydo of the client whose sheet names the sound of its appearance
 * and of its death, the spawner of its effects and a sound player that
 * writes down what it plays, for the cues of an entity.
 */
class EntityCuesFixture : public ::testing::Test {
 protected:
  EntityCuesFixture();

  /** @brief Plays the cues of the Bydo appearing. */
  void announceTheBydo();
  /** @brief Plays the cues of the Bydo killed. */
  void killTheBydo();
  /** @returns The ids of the sounds played, in order. */
  [[nodiscard]] const std::vector<std::string>& played() const;

 private:
  rtype::client::EntityAppearance appearance_;
  rtype::engine::EntityRegistry entities_;
  rtype::engine::ComponentRegistry components_{entities_};
  rtype::client::EffectSpawner effects_;
  RecordingSoundPlayer sounds_;
  rtype::engine::Entity bydo_{};
};

#include <gtest/gtest.h>
#include <chrono>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "HitFlash.hpp"
#include "HitFlashSystem.hpp"
#include "Layer.hpp"
#include "ScreenTestConstants.hpp"
#include "Sprite.hpp"
#include "SpriteFrame.hpp"
#include "SpriteTone.hpp"
#include "TimeConstants.hpp"

using rtype::client::HitFlash;
using rtype::client::HitFlashSystem;
using rtype::client::SpriteTone;
using rtype::engine::ComponentRegistry;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::game::Layer;
using rtype::game::Sprite;
using rtype::game::SpriteFrame;

namespace {

constexpr Duration FLASH_TIME = std::chrono::milliseconds(60);
constexpr Duration LONGER_FLASH_TIME = std::chrono::milliseconds(120);
constexpr SpriteFrame FRAME{.left = 0, .top = 0, .width = 8, .height = 8};
constexpr const char* SHEET_ID = "sheet";

/** @brief Registries of flashed entities, and the system that ends their
 * flashes. */
class FlashedScene {
 public:
  Entity flashed(Duration remaining) {
    const Entity entity = entities_.create();
    components_.add(
        entity,
        Sprite{.assetId = SHEET_ID, .frame = FRAME, .layer = Layer::Enemies});
    components_.add(
        entity, HitFlash{.tone = SpriteTone::Flash, .remaining = remaining});
    return entity;
  }

  void play(Duration elapsed) { system_.update(components_, elapsed); }

  [[nodiscard]] bool isFlashed(Entity entity) const {
    return components_.has<HitFlash>(entity);
  }

  [[nodiscard]] bool isAlive(Entity entity) const {
    return entities_.isAlive(entity);
  }

 private:
  EntityRegistry entities_;
  ComponentRegistry components_{entities_};
  HitFlashSystem system_;
};

}  // namespace

/**
 * Given an entity flashed for 60 ms
 * When the system plays one nanosecond less than 60 ms
 * Then the entity is still flashed
 */
TEST(HitFlashSystem, AFlashLastsUntilItsTimeIsSpent) {
  FlashedScene scene;
  const Entity hit = scene.flashed(FLASH_TIME);

  scene.play(FLASH_TIME - ONE_TICK);

  EXPECT_TRUE(scene.isFlashed(hit));
}

/**
 * Given an entity flashed for 60 ms
 * When the system plays 60 ms
 * Then the entity is no longer flashed
 */
TEST(HitFlashSystem, AFlashEndsOnceItsTimeIsSpent) {
  FlashedScene scene;
  const Entity hit = scene.flashed(FLASH_TIME);

  scene.play(FLASH_TIME);

  EXPECT_FALSE(scene.isFlashed(hit));
}

/**
 * Given an entity flashed for 60 ms
 * When its flash ends
 * Then the entity itself stays alive
 */
TEST(HitFlashSystem, AnEntityWhoseFlashEndedStaysAlive) {
  FlashedScene scene;
  const Entity hit = scene.flashed(FLASH_TIME);

  scene.play(FLASH_TIME);

  EXPECT_TRUE(scene.isAlive(hit));
}

/**
 * Given one entity flashed for 60 ms and another for 120 ms
 * When the system plays 60 ms
 * Then the first is no longer flashed and the second still is
 */
TEST(HitFlashSystem, EachFlashEndsAtItsOwnTime) {
  FlashedScene scene;
  const Entity shortFlash = scene.flashed(FLASH_TIME);
  const Entity longFlash = scene.flashed(LONGER_FLASH_TIME);

  scene.play(FLASH_TIME);

  EXPECT_FALSE(scene.isFlashed(shortFlash));
  EXPECT_TRUE(scene.isFlashed(longFlash));
}

/**
 * Given no flashed entity
 * When the system plays a step
 * Then nothing fails
 */
TEST(HitFlashSystem, AStepWithoutFlashChangesNothing) {
  FlashedScene scene;

  EXPECT_NO_THROW(scene.play(FLASH_TIME));
}

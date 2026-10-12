#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>
#include <vector>
#include "AllEntityTypes.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntitySprites.hpp"
#include "EntityType.hpp"
#include "Position.hpp"
#include "Sprite.hpp"
#include "SpriteFrame.hpp"
#include "SpriteShowcase.hpp"

using rtype::client::spawnSpriteShowcase;
using rtype::client::spriteOf;
using rtype::engine::ComponentRegistry;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::game::EntityType;
using rtype::game::Position;
using rtype::game::Sprite;
using rtype::game::SpriteFrame;
using ::testing::UnorderedElementsAreArray;

/**
 * Given empty registries
 * When the sprite showcase is spawned
 * Then each entity type shows exactly once, with the sprite the entity table
 * gives it
 */
TEST(SpriteShowcase, ShowsEachEntityTypeOnce) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  std::vector<SpriteFrame> expectedFrames;
  expectedFrames.reserve(ALL_ENTITY_TYPES.size());
  for (const EntityType type : ALL_ENTITY_TYPES) {
    const std::optional<Sprite> sprite = spriteOf(type);
    ASSERT_TRUE(sprite.has_value());
    expectedFrames.push_back(sprite.value_or(Sprite{}).frame);
  }

  spawnSpriteShowcase(entities, components);

  std::vector<SpriteFrame> shownFrames;
  components.forEach<Position, Sprite>(
      [&shownFrames](Entity, const Position&, const Sprite& sprite) {
        shownFrames.push_back(sprite.frame);
      });
  EXPECT_THAT(shownFrames, UnorderedElementsAreArray(expectedFrames));
}

#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "EntityRegistry.hpp"
#include "EntityReplicator.hpp"
#include "EntityType.hpp"
#include "Layer.hpp"
#include "Position.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client::testing {

/**
 * @brief An entity as RenderSystem draws it: where it is and the sprite it
 * shows.
 */
struct DrawnEntity {
  float x{};
  float y{};
  std::string assetId;
  game::SpriteFrame frame{};
  game::Layer layer{};

  bool operator==(const DrawnEntity&) const = default;
};

/**
 * @brief Prints the entity in the message of a failed assertion.
 */
void PrintTo(const DrawnEntity& entity, std::ostream* output);

/**
 * @returns How an entity of @p type with the network id @p networkId, at
 * @p position, must be drawn: with the sprite animatedSpriteOf() gives it.
 */
DrawnEntity drawnAs(game::EntityType type, std::uint32_t networkId,
                    game::Position position);

/**
 * @returns How an effect whose first frame is @p firstFrame, centered on
 * @p position, must be drawn when it starts: from the effect sheet, in the
 * Effects layer.
 */
DrawnEntity effectAt(game::SpriteFrame firstFrame, game::Position position);

/**
 * @brief The registries of a game screen, the spawner of its effects, and the
 * replicator that fills them.
 */
class EntityReplicatorFixture : public ::testing::Test {
 protected:
  /**
   * @returns The replicator the tests hand their messages to.
   */
  EntityReplicator& replicator();

  /**
   * @brief Announces one entity of each type of the protocol, each with the
   * id and the position its test constants give it: SHIP_ID at
   * SHIP_POSITION, PLAYER_MISSILE_ID at PLAYER_MISSILE_POSITION, BYDO_ID at
   * BYDO_POSITION, ENEMY_MISSILE_ID at ENEMY_MISSILE_POSITION.
   */
  void announceOneOfEachType();

  /**
   * @returns Every entity of the Entities layer, as RenderSystem would draw
   * it, in no particular order: the ships, Bydos and missiles, not their
   * effects.
   */
  [[nodiscard]] std::vector<DrawnEntity> drawnEntities();

  /**
   * @returns Every effect, in the Effects layer, as RenderSystem would draw
   * it, in no particular order.
   */
  [[nodiscard]] std::vector<DrawnEntity> drawnEffects();

  /**
   * @returns How many entities of the Entities layer play an animation.
   */
  [[nodiscard]] std::size_t animatedEntityCount();

  /**
   * @returns How many entities are alive in the registry, effects included.
   */
  [[nodiscard]] std::size_t entityCount() const;

 private:
  /**
   * @returns Every entity of @p layer that has a Position and a Sprite.
   */
  [[nodiscard]] std::vector<DrawnEntity> drawnIn(game::Layer layer);

  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  EffectSpawner effects_{entities_, components_};
  EntityReplicator replicator_{entities_, components_, effects_};
};

}  // namespace rtype::client::testing

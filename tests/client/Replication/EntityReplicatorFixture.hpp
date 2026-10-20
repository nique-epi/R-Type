#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <ostream>
#include <string>
#include <vector>
#include "ComponentRegistry.hpp"
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
 * @returns How an entity of @p type at @p position must be drawn: with the
 * sprite spriteOf() gives the type.
 */
DrawnEntity drawnAs(game::EntityType type, game::Position position);

/**
 * @brief The registries of a game screen and the replicator that fills them.
 */
class EntityReplicatorFixture : public ::testing::Test {
 protected:
  /**
   * @returns The replicator the tests hand their messages to.
   */
  EntityReplicator& replicator();

  /**
   * @returns Every entity that has a Position and a Sprite, as RenderSystem
   * would draw it, in no particular order.
   */
  [[nodiscard]] std::vector<DrawnEntity> drawnEntities();

  /**
   * @returns How many entities are alive in the registry.
   */
  [[nodiscard]] std::size_t entityCount() const;

 private:
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  EntityReplicator replicator_{entities_, components_};
};

}  // namespace rtype::client::testing

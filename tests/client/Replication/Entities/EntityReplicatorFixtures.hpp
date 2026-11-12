#pragma once

#include <SFML/Graphics/Color.hpp>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <ostream>
#include <string>
#include <vector>
#include "CollisionBox.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "HitboxParts.hpp"
#include "Layer.hpp"
#include "Obstacle.hpp"
#include "Position.hpp"
#include "SpriteFrame.hpp"

namespace rtype::engine {
class ComponentRegistry;
}  // namespace rtype::engine

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
 * @returns How an entity of side @p side with the network id @p networkId, at
 * @p position, must be drawn: with the sprite animatedSpriteOf() gives the
 * appearance appearanceOfSide() gives its side.
 */
DrawnEntity drawnAs(game::EntityType side, std::uint32_t networkId,
                    game::Position position);

/**
 * @returns How an effect whose first frame is @p firstFrame, centered on
 * @p position, must be drawn when it starts: from the effect sheet, in the
 * Effects layer.
 */
DrawnEntity effectAt(game::SpriteFrame firstFrame, game::Position position);

/**
 * @returns Every entity of @p components the server announced, as
 * RenderSystem would draw it, in no particular order, whatever its layer: the
 * ships, Bydos and missiles, not their effects nor the white picture a ship
 * leaves.
 */
[[nodiscard]] std::vector<DrawnEntity> drawnEntities(
    engine::ComponentRegistry& components);

/**
 * @returns Every effect of @p components, in the Effects layer, as
 * RenderSystem would draw it, in no particular order.
 */
[[nodiscard]] std::vector<DrawnEntity> drawnEffects(
    engine::ComponentRegistry& components);

/**
 * @returns How many entities of @p components the server announced play an
 * animation.
 */
[[nodiscard]] std::size_t animatedEntityCount(
    engine::ComponentRegistry& components);

/** @returns The pilot color of the only entity of that type, or nothing
 * when it has none or there is no such entity. */
[[nodiscard]] std::optional<int> pilotColorOfThe(
    engine::ComponentRegistry& components, game::EntityType type);

/** @returns Whether the only entity of that type shows it cannot be hit. */
[[nodiscard]] bool isInvulnerable(engine::ComponentRegistry& components,
                                  game::EntityType type);

/** @returns Whether the only entity of that type is drawn hit. */
[[nodiscard]] bool isFlashed(engine::ComponentRegistry& components,
                             game::EntityType type);

/** @returns Whether the only entity of that type is arriving, hidden or
 * shown row by row. */
[[nodiscard]] bool isArriving(engine::ComponentRegistry& components,
                              game::EntityType type);

/** @returns The colors of the speed lines, in no particular order. */
[[nodiscard]] std::vector<sf::Color> speedLineColors(
    engine::ComponentRegistry& components);

/** @returns How many white pictures of destroyed ships are left. */
[[nodiscard]] std::size_t frozenPictureCount(
    engine::ComponentRegistry& components);

/** @returns How many entities the server announced are alive: those with
 * an EntityType, not their effects. */
[[nodiscard]] std::size_t announcedEntityCount(
    engine::ComponentRegistry& components);

/** @returns Where the history of the only entity that has one puts it at
 * @p tick, nothing when no entity has one. */
[[nodiscard]] std::optional<game::Position> historyPositionAt(
    engine::ComponentRegistry& components, double tick);

/** @returns The box of the only entity of that type, nothing when it has
 * none or there is no such entity. */
[[nodiscard]] std::optional<game::CollisionBox> boxOfThe(
    engine::ComponentRegistry& components, game::EntityType type);

/** @returns The look the entity of network id @p networkId shows, 0 when it
 * shows its idle; nothing when the client has no such entity. */
[[nodiscard]] std::optional<std::uint8_t> shownLookOf(
    engine::ComponentRegistry& components, std::uint32_t networkId);

/** @returns The HitboxParts of the only entity of that type, nothing when it
 * has none or there is no such entity. */
[[nodiscard]] std::optional<game::HitboxParts> partsOfThe(
    engine::ComponentRegistry& components, game::EntityType type);

/** @returns The Obstacle of the only entity of that type, nothing when it has
 * none or there is no such entity. */
[[nodiscard]] std::optional<game::Obstacle> obstacleOfThe(
    engine::ComponentRegistry& components, game::EntityType type);

/** @returns The EntitySpawned of the player missile PLAYER_MISSILE_ID at
 * PLAYER_MISSILE_POSITION, fired by player @p owner with SOME_FIRING_INPUT. */
[[nodiscard]] network::EntitySpawned ownMissileSpawned(std::uint8_t owner);

}  // namespace rtype::client::testing

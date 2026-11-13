#include "OwnMissilesFixture.hpp"
#include <cstdint>
#include <vector>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "FixedEntityTypeSourceConstants.hpp"
#include "HitboxParts.hpp"
#include "OwnMissiles.hpp"
#include "OwnMissilesTestConstants.hpp"
#include "Position.hpp"
#include "PredictedCommand.hpp"
#include "RecordingSoundPlayer.hpp"
#include "Sprite.hpp"

namespace rtype::client::testing {

OwnMissilesFixture::OwnMissilesFixture() {
  missiles_.activate();
  missiles_.restart(NOTHING_ACKNOWLEDGED, SHIP_WHEN_IT_FIRES);
}

void OwnMissilesFixture::fireAt(std::uint32_t input, game::Position ship) {
  const std::vector<PredictedCommand> commands{PredictedCommand{
      .inputNumber = input, .shooting = true, .shipAfter = ship}};
  missiles_.follow(commands, true);
  newestInput_ = input;
}

void OwnMissilesFixture::holdNothingUpTo(std::uint32_t input) {
  std::vector<PredictedCommand> commands;
  for (std::uint32_t next = newestInput_ + 1; next <= input; ++next) {
    commands.push_back(PredictedCommand{
        .inputNumber = next, .shooting = false, .shipAfter = {}});
  }
  missiles_.follow(commands, true);
  newestInput_ = input;
}

void OwnMissilesFixture::holdShootUpTo(std::uint32_t input) {
  std::vector<PredictedCommand> commands;
  for (std::uint32_t next = newestInput_ + 1; next <= input; ++next) {
    commands.push_back(PredictedCommand{.inputNumber = next,
                                        .shooting = true,
                                        .shipAfter = SHIP_WHEN_IT_FIRES});
  }
  missiles_.follow(commands, true);
  newestInput_ = input;
}

std::vector<game::Position> OwnMissilesFixture::drawnMissiles() {
  std::vector<game::Position> drawn;
  components_.forEach<game::EntityType, game::Position>(
      [this, &drawn](engine::Entity entity, const game::EntityType& side,
                     const game::Position& position) {
        if (side == game::EntityType::PlayerMissile &&
            components_.has<game::Sprite>(entity)) {
          drawn.push_back(position);
        }
      });
  return drawn;
}

void OwnMissilesFixture::enemyAt(game::Position position) {
  const engine::Entity enemy = entities_.create();
  components_.add(enemy, position);
  components_.add(enemy, game::EntityType::Enemy);
  components_.add(enemy, BYDO_BOX);
  components_.add(enemy, game::Sprite{});
}

void OwnMissilesFixture::enemyOfTwoBoxesAt(game::Position position) {
  const engine::Entity enemy = entities_.create();
  components_.add(enemy, position);
  components_.add(enemy, game::EntityType::Enemy);
  components_.add(enemy, BYDO_BOX);
  components_.add(enemy,
                  game::HitboxParts{.boxes = {TWO_BOX_ENEMY_PARTS.begin(),
                                              TWO_BOX_ENEMY_PARTS.end()}});
  components_.add(enemy, game::Sprite{});
}

OwnMissiles& OwnMissilesFixture::missiles() { return missiles_; }

const RecordingSoundPlayer& OwnMissilesFixture::sounds() const {
  return sounds_;
}

}  // namespace rtype::client::testing

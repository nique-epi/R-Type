#pragma once

#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @brief Destroys every enemy that went out of the playfield on the left, and
 * announces each one with an EntityDestruction Removed.
 *
 * An enemy is out when its Position is more than BYDO_EXIT_MARGIN beyond the
 * left edge: exactly BYDO_EXIT_MARGIN beyond is still in. Only entities of
 * EntityType::Enemy that have a NetworkIdentity are removed: a ship is never
 * removed by leaving. The bus must outlive this object.
 */
class PlayfieldExitSystem final : public engine::ISystem {
 public:
  explicit PlayfieldExitSystem(engine::EventBus& events);

  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;

 private:
  engine::EventBus& events_;
};

}  // namespace rtype::game

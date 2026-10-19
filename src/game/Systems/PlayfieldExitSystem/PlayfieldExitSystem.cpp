#include "PlayfieldExitSystem.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "NetworkIdentity.hpp"
#include "Position.hpp"
#include "SpawningConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

PlayfieldExitSystem::PlayfieldExitSystem(engine::EventBus& events)
    : events_(events) {}

void PlayfieldExitSystem::update(engine::ComponentRegistry& components,
                                 engine::Duration /*elapsed*/) {
  components.forEach<Position, EntityType>(
      [this, &components](engine::Entity entity, const Position& position,
                          const EntityType& type) {
        if (type != EntityType::Enemy || position.x >= -BYDO_EXIT_MARGIN) {
          return;
        }
        const NetworkIdentity* identity =
            components.get<NetworkIdentity>(entity);
        if (identity == nullptr) {
          return;
        }
        events_.publish(EntityDestruction{.networkId = identity->networkId,
                                          .cause = DestructionCause::Removed});
        components.destroy(entity);
      });
}

}  // namespace rtype::game

#include "EventBusFixtures.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EventBus.hpp"
#include "TimeConstants.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::engine::EventBus;

ContactSystem::ContactSystem(EventBus& events, Entity missile)
    : events_(&events), missile_(missile) {}

void ContactSystem::update(ComponentRegistry& components,
                           [[maybe_unused]] Duration elapsed) {
  components.forEach<Health, Health>([this](Entity target, Health&, Health&) {
    events_->publish(Collision{.first = missile_, .second = target});
  });
}

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include "ComponentRegistry.hpp"
#include "EngineException.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EventBus.hpp"
#include "ISystem.hpp"
#include "SubscriptionHandle.hpp"
#include "SystemScheduler.hpp"
#include "TimeConstants.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::DeadEntityException;
using rtype::engine::Duration;
using rtype::engine::EmptyEventCallbackException;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::engine::EventBus;
using rtype::engine::ISystem;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::engine::SubscriptionHandle;
using rtype::engine::SystemScheduler;

namespace {

struct Collision {
  Entity first;
  Entity second;

  bool operator==(const Collision&) const = default;
};

struct EntityDestroyed {
  Entity entity;
};

struct Health {
  int points;
};

struct Explosion {
  Entity source;
};

constexpr Entity FIRST_ENTITY{.index = 1, .generation = 0};
constexpr Entity SECOND_ENTITY{.index = 2, .generation = 0};
constexpr Entity THIRD_ENTITY{.index = 3, .generation = 0};
constexpr int FIRST_LABEL = 1;
constexpr int SECOND_LABEL = 2;
constexpr int THIRD_LABEL = 3;
constexpr int FULL_HEALTH = 100;
constexpr int MISSILE_DAMAGE = 1;

/**
 * @brief Publishes, during its forEach, a Collision between the missile and
 * every entity that has a Health, like a collision system would.
 */
class ContactSystem final : public ISystem {
 public:
  ContactSystem(EventBus& events, Entity missile)
      : events_(&events), missile_(missile) {}

  void update(ComponentRegistry& components,
              [[maybe_unused]] Duration elapsed) override {
    components.forEach<Health, Health>([this](Entity target, Health&, Health&) {
      events_->publish(Collision{.first = missile_, .second = target});
    });
  }

 private:
  EventBus* events_;
  Entity missile_;
};

}  // namespace

/**
 * Given a subscriber to Collision
 * When a Collision is published, then dispatched
 * Then the subscriber received that Collision once
 */
TEST(EventBus, DeliversAPublishedEventToItsSubscriber) {
  EventBus events;
  std::vector<Collision> received;
  events.subscribe<Collision>([&received](const Collision& collision) {
    received.push_back(collision);
  });

  events.publish(Collision{.first = FIRST_ENTITY, .second = SECOND_ENTITY});
  events.dispatch();

  EXPECT_EQ(received, (std::vector<Collision>{Collision{
                          .first = FIRST_ENTITY, .second = SECOND_ENTITY}}));
}

/**
 * Given a subscriber to Collision
 * When a Collision is published and dispatch() is not called
 * Then the subscriber has not been called
 */
TEST(EventBus, DeliversNothingBeforeDispatch) {
  EventBus events;
  int calls = 0;
  events.subscribe<Collision>([&calls](const Collision&) { ++calls; });

  events.publish(Collision{.first = FIRST_ENTITY, .second = SECOND_ENTITY});

  EXPECT_EQ(calls, 0);
}

/**
 * Given a subscriber to Collision and a subscriber to EntityDestroyed
 * When an EntityDestroyed is published, then dispatched
 * Then only the subscriber to EntityDestroyed is called
 */
TEST(EventBus, DeliversAnEventOnlyToSubscribersOfItsType) {
  EventBus events;
  int collisionCalls = 0;
  int destructionCalls = 0;
  events.subscribe<Collision>(
      [&collisionCalls](const Collision&) { ++collisionCalls; });
  events.subscribe<EntityDestroyed>(
      [&destructionCalls](const EntityDestroyed&) { ++destructionCalls; });

  events.publish(EntityDestroyed{.entity = FIRST_ENTITY});
  events.dispatch();

  EXPECT_EQ(collisionCalls, 0);
  EXPECT_EQ(destructionCalls, 1);
}

/**
 * Given three subscribers to Collision, subscribed in a given order
 * When a Collision is published, then dispatched
 * Then they are called in the order they subscribed
 */
TEST(EventBus, CallsSubscribersInTheOrderTheySubscribed) {
  EventBus events;
  std::vector<int> journal;
  events.subscribe<Collision>(
      [&journal](const Collision&) { journal.push_back(FIRST_LABEL); });
  events.subscribe<Collision>(
      [&journal](const Collision&) { journal.push_back(SECOND_LABEL); });
  events.subscribe<Collision>(
      [&journal](const Collision&) { journal.push_back(THIRD_LABEL); });

  events.publish(Collision{.first = FIRST_ENTITY, .second = SECOND_ENTITY});
  events.dispatch();

  EXPECT_EQ(journal,
            (std::vector<int>{FIRST_LABEL, SECOND_LABEL, THIRD_LABEL}));
}

/**
 * Given subscribers to Collision and to EntityDestroyed that record the entity
 * they receive
 * When a Collision, an EntityDestroyed and another Collision are published,
 * then dispatched
 * Then the entities are recorded in the order the events were published
 */
TEST(EventBus, DeliversEventsInTheOrderTheyWerePublished) {
  EventBus events;
  std::vector<Entity> delivered;
  events.subscribe<Collision>([&delivered](const Collision& collision) {
    delivered.push_back(collision.first);
  });
  events.subscribe<EntityDestroyed>(
      [&delivered](const EntityDestroyed& destroyed) {
        delivered.push_back(destroyed.entity);
      });

  events.publish(Collision{.first = FIRST_ENTITY, .second = THIRD_ENTITY});
  events.publish(EntityDestroyed{.entity = SECOND_ENTITY});
  events.publish(Collision{.first = THIRD_ENTITY, .second = FIRST_ENTITY});
  events.dispatch();

  EXPECT_EQ(delivered,
            (std::vector<Entity>{FIRST_ENTITY, SECOND_ENTITY, THIRD_ENTITY}));
}

/**
 * Given a subscriber to Collision that publishes an EntityDestroyed, and a
 * subscriber to EntityDestroyed
 * When a Collision is published, then dispatched once
 * Then the EntityDestroyed has been delivered by that same dispatch
 */
TEST(EventBus, EventPublishedDuringDispatchIsDeliveredInTheSameDispatch) {
  EventBus events;
  std::vector<Entity> destroyed;
  events.subscribe<Collision>([&events](const Collision& collision) {
    events.publish(EntityDestroyed{.entity = collision.second});
  });
  events.subscribe<EntityDestroyed>(
      [&destroyed](const EntityDestroyed& destruction) {
        destroyed.push_back(destruction.entity);
      });

  events.publish(Collision{.first = FIRST_ENTITY, .second = SECOND_ENTITY});
  events.dispatch();

  EXPECT_EQ(destroyed, std::vector<Entity>{SECOND_ENTITY});
}

/**
 * Given a Collision published and dispatched while nobody subscribed to
 * Collision
 * When a subscriber to Collision is added and the bus dispatches again
 * Then that subscriber is not called
 */
TEST(EventBus, EventWithoutSubscriberIsDroppedByDispatch) {
  EventBus events;
  events.publish(Collision{.first = FIRST_ENTITY, .second = SECOND_ENTITY});
  events.dispatch();
  int calls = 0;
  events.subscribe<Collision>([&calls](const Collision&) { ++calls; });

  events.dispatch();

  EXPECT_EQ(calls, 0);
}

/**
 * Given a subscriber to Collision and a Collision waiting in the queue
 * When the subscriber unsubscribes, then the bus dispatches
 * Then the subscriber is not called
 */
TEST(EventBus, UnsubscribedCallbackMissesEventsStillInTheQueue) {
  EventBus events;
  int calls = 0;
  const SubscriptionHandle handle =
      events.subscribe<Collision>([&calls](const Collision&) { ++calls; });
  events.publish(Collision{.first = FIRST_ENTITY, .second = SECOND_ENTITY});

  events.unsubscribe(handle);
  events.dispatch();

  EXPECT_EQ(calls, 0);
}

/**
 * Given a subscription
 * When it is unsubscribed twice
 * Then the first call reports a removal and the second does not
 */
TEST(EventBus, UnsubscribeReportsWhetherTheSubscriptionExisted) {
  EventBus events;
  const SubscriptionHandle handle =
      events.subscribe<Collision>([](const Collision&) {});

  const bool firstRemoval = events.unsubscribe(handle);
  const bool secondRemoval = events.unsubscribe(handle);

  EXPECT_TRUE(firstRemoval);
  EXPECT_FALSE(secondRemoval);
}

/**
 * Given three subscribers to Collision, the second one unsubscribing itself
 * when it is called
 * When two Collisions are published, then dispatched
 * Then the third is called for both, and the second only for the first
 */
TEST(EventBus, SubscriberRemovingItselfDoesNotSkipTheNextOne) {
  EventBus events;
  std::vector<int> journal;
  SubscriptionHandle secondHandle{};
  events.subscribe<Collision>(
      [&journal](const Collision&) { journal.push_back(FIRST_LABEL); });
  secondHandle = events.subscribe<Collision>(
      [&journal, &events, &secondHandle](const Collision&) {
        journal.push_back(SECOND_LABEL);
        events.unsubscribe(secondHandle);
      });
  events.subscribe<Collision>(
      [&journal](const Collision&) { journal.push_back(THIRD_LABEL); });

  events.publish(Collision{.first = FIRST_ENTITY, .second = SECOND_ENTITY});
  events.publish(Collision{.first = FIRST_ENTITY, .second = THIRD_ENTITY});
  events.dispatch();

  EXPECT_EQ(journal, (std::vector<int>{FIRST_LABEL, SECOND_LABEL, THIRD_LABEL,
                                       FIRST_LABEL, THIRD_LABEL}));
}

/**
 * Given a subscriber to Collision that subscribes a second callback the first
 * time it is called
 * When two Collisions are published, then dispatched
 * Then the second callback only receives the second Collision
 */
TEST(EventBus, SubscriberAddedDuringDeliveryOnlyReceivesLaterEvents) {
  EventBus events;
  std::vector<int> journal;
  bool secondSubscribed = false;
  events.subscribe<Collision>(
      [&journal, &events, &secondSubscribed](const Collision&) {
        journal.push_back(FIRST_LABEL);
        if (!secondSubscribed) {
          secondSubscribed = true;
          events.subscribe<Collision>([&journal](const Collision&) {
            journal.push_back(SECOND_LABEL);
          });
        }
      });

  events.publish(Collision{.first = FIRST_ENTITY, .second = SECOND_ENTITY});
  events.publish(Collision{.first = FIRST_ENTITY, .second = THIRD_ENTITY});
  events.dispatch();

  EXPECT_EQ(journal,
            (std::vector<int>{FIRST_LABEL, FIRST_LABEL, SECOND_LABEL}));
}

/**
 * Given an empty bus
 * When an empty callback subscribes to Collision
 * Then it is refused, and a later Collision is dispatched without error
 */
TEST(EventBus, RejectsAnEmptyCallback) {
  EventBus events;

  EXPECT_THROW(events.subscribe<Collision>(nullptr),
               EmptyEventCallbackException);

  events.publish(Collision{.first = FIRST_ENTITY, .second = SECOND_ENTITY});
  EXPECT_NO_THROW(events.dispatch());
}

/**
 * Given a subscriber to Collision that throws, and a subscriber to
 * EntityDestroyed
 * When a Collision then an EntityDestroyed are published, and the bus
 * dispatches twice
 * Then the first dispatch throws, and the EntityDestroyed is delivered by the
 * second
 */
TEST(EventBus, EventsAfterAThrowingSubscriberWaitForTheNextDispatch) {
  EventBus events;
  std::vector<Entity> destroyed;
  events.subscribe<Collision>(
      [](const Collision&) { throw DeadEntityException(); });
  events.subscribe<EntityDestroyed>(
      [&destroyed](const EntityDestroyed& destruction) {
        destroyed.push_back(destruction.entity);
      });
  events.publish(Collision{.first = FIRST_ENTITY, .second = SECOND_ENTITY});
  events.publish(EntityDestroyed{.entity = SECOND_ENTITY});

  EXPECT_THROW(events.dispatch(), DeadEntityException);
  events.dispatch();

  EXPECT_EQ(destroyed, std::vector<Entity>{SECOND_ENTITY});
}

/**
 * Given a system that publishes a Collision from inside its forEach, and a
 * subscriber that creates an explosion entity with a component for each
 * Collision
 * When the system runs, then the bus dispatches
 * Then the explosion entity carries its component
 */
TEST(EventBus, SubscriberMayAddComponentsForEventsPublishedInsideAQuery) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  EventBus events;
  const Entity missile = entities.create();
  const Entity bydo = entities.create();
  components.add(bydo, Health{.points = FULL_HEALTH});
  std::vector<Entity> explosions;
  events.subscribe<Collision>(
      [&entities, &components, &explosions](const Collision& collision) {
        const Entity explosion = entities.create();
        components.add(explosion, Explosion{.source = collision.second});
        explosions.push_back(explosion);
      });
  SystemScheduler systems;
  systems.add(std::make_unique<ContactSystem>(events, missile));

  systems.run(components, SIMULATION_TICK_DURATION);
  events.dispatch();

  ASSERT_EQ(explosions.size(), 1U);
  const Explosion* explosion = components.get<Explosion>(explosions.front());
  ASSERT_NE(explosion, nullptr);
  EXPECT_EQ(explosion->source, bydo);
}

/**
 * Given a system that publishes a Collision when the missile touches the
 * Bydo, a damage subscriber that destroys the Bydo and publishes an
 * EntityDestroyed when its health runs out, and a subscriber that records what
 * the network would announce
 * When one tick runs: the systems, then one dispatch
 * Then the destruction of the Bydo is recorded for announcement
 */
TEST(EventBus, CollisionDamageAndAnnouncementCompleteInOneTick) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  EventBus events;
  const Entity missile = entities.create();
  const Entity bydo = entities.create();
  components.add(bydo, Health{.points = MISSILE_DAMAGE});
  events.subscribe<Collision>(
      [&components, &events](const Collision& collision) {
        auto* health = components.get<Health>(collision.second);
        if (health == nullptr) {
          return;
        }
        health->points -= MISSILE_DAMAGE;
        if (health->points <= 0) {
          components.destroy(collision.second);
          events.publish(EntityDestroyed{.entity = collision.second});
        }
      });
  std::vector<Entity> announced;
  events.subscribe<EntityDestroyed>(
      [&announced](const EntityDestroyed& destruction) {
        announced.push_back(destruction.entity);
      });
  SystemScheduler systems;
  systems.add(std::make_unique<ContactSystem>(events, missile));

  systems.run(components, SIMULATION_TICK_DURATION);
  events.dispatch();

  EXPECT_EQ(announced, std::vector<Entity>{bydo});
}

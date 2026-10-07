#include <gtest/gtest.h>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
#include "ComponentRegistry.hpp"
#include "EngineException.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "ISystem.hpp"
#include "SystemScheduler.hpp"
#include "TimeConstants.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::engine::ISystem;
using rtype::engine::NullSystemException;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::engine::SystemScheduler;

namespace {

struct Health {
  int points;
};

constexpr int FIRST_LABEL = 1;
constexpr int SECOND_LABEL = 2;
constexpr int THIRD_LABEL = 3;
constexpr int FULL_HEALTH = 100;
constexpr std::size_t ENTITY_COUNT = 5;

class RecordingSystem final : public ISystem {
 public:
  RecordingSystem(std::vector<int>& journal, int label)
      : journal_(&journal), label_(label) {}

  void update(ComponentRegistry& components, Duration elapsed) override {
    journal_->push_back(label_);
    receivedComponents_ = &components;
    receivedElapsed_ = elapsed;
  }

  [[nodiscard]] const ComponentRegistry* receivedComponents() const {
    return receivedComponents_;
  }

  [[nodiscard]] Duration receivedElapsed() const { return receivedElapsed_; }

 private:
  std::vector<int>* journal_;
  int label_;
  const ComponentRegistry* receivedComponents_{nullptr};
  Duration receivedElapsed_{};
};

class DestroyingSystem final : public ISystem {
 public:
  void update(ComponentRegistry& components,
              [[maybe_unused]] Duration elapsed) override {
    components.forEach<Health, Health>(
        [&components](Entity entity, Health&, Health&) {
          components.destroy(entity);
        });
  }
};

class CountingSystem final : public ISystem {
 public:
  explicit CountingSystem(std::size_t& visits) : visits_(&visits) {}

  void update(ComponentRegistry& components,
              [[maybe_unused]] Duration elapsed) override {
    components.forEach<Health, Health>(
        [this](Entity, Health&, Health&) { ++*visits_; });
  }

 private:
  std::size_t* visits_;
};

}  // namespace

/**
 * Given three systems added in a given order
 * When the scheduler runs
 * Then the systems are called in the order they were added
 */
TEST(SystemScheduler, RunsSystemsInTheOrderTheyWereAdded) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  std::vector<int> journal;
  SystemScheduler scheduler;
  scheduler.add(std::make_unique<RecordingSystem>(journal, FIRST_LABEL));
  scheduler.add(std::make_unique<RecordingSystem>(journal, SECOND_LABEL));
  scheduler.add(std::make_unique<RecordingSystem>(journal, THIRD_LABEL));

  scheduler.run(components, SIMULATION_TICK_DURATION);

  EXPECT_EQ(journal,
            (std::vector<int>{FIRST_LABEL, SECOND_LABEL, THIRD_LABEL}));
}

/**
 * Given three systems added in a given order
 * When the scheduler runs three times
 * Then every run calls the systems in the same order
 */
TEST(SystemScheduler, OrderIsTheSameOnEveryRun) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  std::vector<int> journal;
  SystemScheduler scheduler;
  scheduler.add(std::make_unique<RecordingSystem>(journal, FIRST_LABEL));
  scheduler.add(std::make_unique<RecordingSystem>(journal, SECOND_LABEL));
  scheduler.add(std::make_unique<RecordingSystem>(journal, THIRD_LABEL));

  scheduler.run(components, SIMULATION_TICK_DURATION);
  scheduler.run(components, SIMULATION_TICK_DURATION);
  scheduler.run(components, SIMULATION_TICK_DURATION);

  const std::vector<int> expected{FIRST_LABEL, SECOND_LABEL, THIRD_LABEL,
                                  FIRST_LABEL, SECOND_LABEL, THIRD_LABEL,
                                  FIRST_LABEL, SECOND_LABEL, THIRD_LABEL};
  EXPECT_EQ(journal, expected);
}

/**
 * Given a system added to the scheduler
 * When the scheduler runs with a registry and an elapsed time
 * Then the system receives that registry and that elapsed time
 */
TEST(SystemScheduler, GivesEachSystemTheRegistryAndTheElapsedTime) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  std::vector<int> journal;
  auto system = std::make_unique<RecordingSystem>(journal, FIRST_LABEL);
  const RecordingSystem& recording = *system;
  SystemScheduler scheduler;
  scheduler.add(std::move(system));

  scheduler.run(components, SIMULATION_TICK_DURATION);

  EXPECT_EQ(recording.receivedComponents(), &components);
  EXPECT_EQ(recording.receivedElapsed(), SIMULATION_TICK_DURATION);
}

/**
 * Given an empty scheduler
 * When a null system is added
 * Then it is refused and the scheduler still holds no system
 */
TEST(SystemScheduler, RejectsANullSystem) {
  SystemScheduler scheduler;

  EXPECT_THROW(scheduler.add(nullptr), NullSystemException);
  EXPECT_EQ(scheduler.size(), 0U);
}

/**
 * Given entities with a Health, a system that destroys them and a system added
 * after it that counts them
 * When the scheduler runs
 * Then the entities are destroyed and the second system finds none
 */
TEST(SystemScheduler, EntitiesDestroyedByAnEarlierSystemAreGoneForTheNextOne) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  for (std::size_t count = 0; count < ENTITY_COUNT; ++count) {
    components.add<Health>(entities.create(), {.points = FULL_HEALTH});
  }
  std::size_t visitsAfterDestruction = 0;
  SystemScheduler scheduler;
  scheduler.add(std::make_unique<DestroyingSystem>());
  scheduler.add(std::make_unique<CountingSystem>(visitsAfterDestruction));

  scheduler.run(components, SIMULATION_TICK_DURATION);

  EXPECT_EQ(visitsAfterDestruction, 0U);
  EXPECT_EQ(entities.size(), 0U);
}

#pragma once

#include <optional>
#include <span>
#include <string>
#include "EventBus.hpp"
#include "LuauEventBindingsParts.hpp"
#include "LuauEventListener.hpp"
#include "LuauThread.hpp"
#include "ScriptEvent.hpp"
#include "ScriptEventField.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::game {

/**
 * @brief Gives the scripts of a machine the event bus of the game in
 * progress, as the table `bus`: `bus.on(name, function)` subscribes a function
 * for the entity that steps, and `bus.emit(name, fields)` publishes a
 * ScriptEvent.
 *
 * Each `bus.on` is one subscription to the EventBus, so the bus calls the
 * functions of the scripts in its own order, among its other subscribers,
 * when the game loop dispatches. The name is one of GAME_EVENT_NAMES, whose
 * function reads a table of the event, or any other name, whose function
 * reads the values a script published under it. The subscription is kept in
 * the behaviour state of its entity and ends with it.
 *
 * A subscribed function costs a step of the machine, counted in the budget of
 * the tick. One that fails, waits or runs past its budget stops the script of
 * its entity as a failed step does, and is reported once; the subscribers
 * after it are still called. Once the budget of a tick is spent, an event
 * reaches no function, which also ends events that answer each other.
 *
 * Neither copied nor moved: the machine reaches it by its address.
 */
class LuauEventBindings {
 public:
  /** @brief Exposes the table `bus` on the machine of @p parts, which must
   * not have run a script yet. */
  explicit LuauEventBindings(LuauEventBindingsParts parts);
  LuauEventBindings(const LuauEventBindings&) = delete;
  LuauEventBindings& operator=(const LuauEventBindings&) = delete;
  LuauEventBindings(LuauEventBindings&&) = delete;
  LuauEventBindings& operator=(LuauEventBindings&&) = delete;
  ~LuauEventBindings() = default;

  /** @brief Makes @p bus the one the scripts subscribe to and publish on; it
   * must outlive every entity that subscribed. */
  void attach(engine::EventBus& bus);

  /** @brief Forgets @p bus, unless another took its place. */
  void detach(const engine::EventBus& bus) noexcept;

  /**
   * @brief Subscribes, for the entity that steps, the function whose
   * reference in the registry is @p function to the event @p name. It takes
   * the reference over, and releases it when it refuses.
   * @throws engine::ScriptException outside the step of an entity, outside a
   * game, or past MAXIMUM_SUBSCRIPTIONS_PER_ENTITY.
   */
  void subscribe(const std::string& name, int function);

  /**
   * @brief Queues @p event on the bus until its next dispatch.
   * @throws engine::ScriptException outside a game, or under one of
   * GAME_EVENT_NAMES.
   */
  void publish(ScriptEvent event);

 private:
  template <typename Event>
  [[nodiscard]] engine::SubscriptionHandle subscribeTo(
      LuauEventListener listener);
  [[nodiscard]] engine::SubscriptionHandle subscribeNamed(
      const std::string& name, LuauEventListener listener);
  void deliver(const LuauEventListener& listener,
               std::span<const ScriptEventField> fields);
  void stopSubscriber(LuauBehaviourState& subscriber,
                      const std::string& reason);

  LuauEventBindingsParts parts_;
  engine::EventBus* bus_{nullptr};
  /** The thread every subscribed function runs on, one at a time; empty
   * before the first one and after one that failed. */
  std::optional<engine::LuauThread> deliveryThread_;
  bool failureReported_{false};
};

}  // namespace rtype::game

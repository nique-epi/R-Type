#include "LuauEventBindings.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>
#include "BossHealthChanged.hpp"
#include "EnemyActionStarted.hpp"
#include "EngineException.hpp"
#include "EntityDamage.hpp"
#include "EntityDestruction.hpp"
#include "EventBus.hpp"
#include "GameScriptingConstants.hpp"
#include "LuauEventConstants.hpp"
#include "LuauEventSubscription.hpp"
#include "LuauEventSubscriptionParts.hpp"
#include "LuauEventSubscriptions.hpp"
#include "LuauProtectedCall.hpp"
#include "LuauResumeResult.hpp"
#include "LuauThread.hpp"
#include "ScriptEvent.hpp"
#include "ScriptEventField.hpp"
#include "ScriptFunction.hpp"
#include "ScriptMachine.hpp"
#include "SubscriptionHandle.hpp"
#include "lua.h"
#include "lualib.h"

namespace rtype::game {

namespace {

/** @brief What pushing a subscribed function and its event needs. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct PendingDelivery {
  std::reference_wrapper<std::optional<engine::LuauThread>> thread;
  lua_State* machine{nullptr};
  int function{0};
  std::span<const ScriptEventField> fields;
  lua_State* deliveryThread{nullptr};
};

/** @returns The LuauEventBindings in the upvalue of the running function. */
LuauEventBindings& bindingsOf(lua_State* state) {
  return *static_cast<LuauEventBindings*>(
      lua_touserdata(state, lua_upvalueindex(1)));
}

std::vector<ScriptEventField> fieldsOf(const EntityDestruction& event) {
  return {ScriptEventField{.key = EVENT_ENTITY_ID_KEY,
                           .value = static_cast<double>(event.networkId)},
          ScriptEventField{.key = EVENT_KILLED_KEY,
                           .value = event.cause == DestructionCause::Killed}};
}

std::vector<ScriptEventField> fieldsOf(const EntityDamage& event) {
  return {ScriptEventField{.key = EVENT_ENTITY_ID_KEY,
                           .value = static_cast<double>(event.networkId)}};
}

std::vector<ScriptEventField> fieldsOf(const EnemyActionStarted& event) {
  return {ScriptEventField{.key = EVENT_ENTITY_ID_KEY,
                           .value = static_cast<double>(event.networkId)},
          ScriptEventField{.key = EVENT_ATTACK_KEY,
                           .value = static_cast<double>(event.attack)},
          ScriptEventField{.key = EVENT_PHASE_KEY,
                           .value = std::string{PHASE_NAMES.at(
                               static_cast<std::size_t>(event.phase))}}};
}

std::vector<ScriptEventField> fieldsOf(const BossHealthChanged& event) {
  return {ScriptEventField{.key = EVENT_ENTITY_ID_KEY,
                           .value = static_cast<double>(event.networkId)},
          ScriptEventField{.key = EVENT_POINTS_KEY,
                           .value = static_cast<double>(event.points)},
          ScriptEventField{.key = EVENT_FULL_POINTS_KEY,
                           .value = static_cast<double>(event.full)},
          ScriptEventField{.key = EVENT_ENRAGED_KEY, .value = event.enraged}};
}

/** @brief Pushes on @p state the value of @p field. */
void pushValue(lua_State* state, const ScriptEventField& field) {
  if (const auto* const number = std::get_if<double>(&field.value)) {
    lua_pushnumber(state, *number);
  } else if (const auto* const flag = std::get_if<bool>(&field.value)) {
    lua_pushboolean(state, *flag ? 1 : 0);
  } else {
    const auto& text = std::get<std::string>(field.value);
    lua_pushlstring(state, text.data(), text.size());
  }
}

/** @brief Pushes, on the delivery thread created when there is none, the
 * function of a delivery and the table of its event. */
void pushDelivery(void* context) {
  auto& pending = *static_cast<PendingDelivery*>(context);
  std::optional<engine::LuauThread>& thread = pending.thread.get();
  if (!thread.has_value()) {
    thread.emplace(pending.machine);
  }
  pending.deliveryThread = thread->state();
  lua_getref(pending.machine, pending.function);
  lua_createtable(pending.machine, 0, static_cast<int>(pending.fields.size()));
  for (const ScriptEventField& field : pending.fields) {
    pushValue(pending.machine, field);
    lua_setfield(pending.machine, -2, field.key.c_str());
  }
  lua_xmove(pending.machine, pending.deliveryThread, 2);
}

/** @returns The field whose key and value are on top of @p state.
 * @throws engine::ScriptException when the key is no string, or the value
 * neither a number, a boolean nor a string. */
ScriptEventField fieldOnTop(lua_State* state) {
  if (lua_type(state, -2) != LUA_TSTRING) {
    throw engine::ScriptException(WRONG_EVENT_FIELD_MESSAGE);
  }
  std::string key = lua_tostring(state, -2);
  switch (lua_type(state, -1)) {
    case LUA_TNUMBER:
      return {.key = std::move(key), .value = lua_tonumber(state, -1)};
    case LUA_TBOOLEAN:
      return {.key = std::move(key), .value = lua_toboolean(state, -1) != 0};
    case LUA_TSTRING:
      return {.key = std::move(key),
              .value = std::string{lua_tostring(state, -1)}};
    default:
      throw engine::ScriptException(WRONG_EVENT_FIELD_MESSAGE);
  }
}

/** @returns The values of the table at EVENT_FIELDS_ARGUMENT, read with no
 * metamethod; none when the argument is left out.
 * @throws engine::ScriptException past MAXIMUM_SCRIPT_EVENT_FIELDS, or as
 * fieldOnTop() does. */
std::vector<ScriptEventField> publishedFields(lua_State* state) {
  std::vector<ScriptEventField> fields;
  if (lua_isnoneornil(state, EVENT_FIELDS_ARGUMENT)) {
    return fields;
  }
  luaL_checktype(state, EVENT_FIELDS_ARGUMENT, LUA_TTABLE);
  lua_pushnil(state);
  while (lua_next(state, EVENT_FIELDS_ARGUMENT) != 0) {
    if (fields.size() >= MAXIMUM_SCRIPT_EVENT_FIELDS) {
      throw engine::ScriptException(TOO_MANY_EVENT_FIELDS_MESSAGE);
    }
    fields.push_back(fieldOnTop(state));
    lua_pop(state, 1);
  }
  return fields;
}

/** @returns Why @p subscriber, the entity that steps, may not subscribe one
 * more function to @p bus; nullptr when it may. */
const char* subscriptionRefusal(const engine::EventBus* bus,
                                const LuauEventSubscriptions* subscriber) {
  if (bus == nullptr || subscriber == nullptr) {
    return SUBSCRIPTION_OUTSIDE_ENTITY_MESSAGE;
  }
  if (subscriber->full()) {
    return TOO_MANY_SUBSCRIPTIONS_MESSAGE;
  }
  return nullptr;
}

/** @brief `bus.on(name, function)`: an exception that leaves it is an error
 * of the script, as Luau makes of any exception a C function throws. */
int subscribeFunction(lua_State* state) {
  const std::string name = luaL_checkstring(state, EVENT_NAME_ARGUMENT);
  luaL_checktype(state, EVENT_FUNCTION_ARGUMENT, LUA_TFUNCTION);
  bindingsOf(state).subscribe(name, lua_ref(state, EVENT_FUNCTION_ARGUMENT));
  return 0;
}

/** @brief `bus.emit(name, fields)`: an exception that leaves it is an error
 * of the script, as for `bus.on`. */
int publishEvent(lua_State* state) {
  std::string name = luaL_checkstring(state, EVENT_NAME_ARGUMENT);
  bindingsOf(state).publish(
      ScriptEvent{.name = std::move(name), .fields = publishedFields(state)});
  return 0;
}

}  // namespace

LuauEventBindings::LuauEventBindings(engine::ScriptMachine& machine)
    : machine_(machine) {
  const std::array functions{
      engine::ScriptFunction{.name = ON_FUNCTION_NAME,
                             .function = subscribeFunction},
      engine::ScriptFunction{.name = EMIT_FUNCTION_NAME,
                             .function = publishEvent}};
  machine.expose(EVENT_BUS_TABLE, functions, this);
}

void LuauEventBindings::attach(engine::EventBus& bus) { bus_ = &bus; }

void LuauEventBindings::detach(const engine::EventBus& bus) noexcept {
  if (bus_ == &bus) {
    bus_ = nullptr;
  }
}

void LuauEventBindings::setSubscriber(LuauEventSubscriptions* subscriber) {
  subscriber_ = subscriber;
}

void LuauEventBindings::subscribe(const std::string& name, int function) {
  lua_State* const machine = machine_.get().state();
  const char* const refusal = subscriptionRefusal(bus_, subscriber_);
  if (refusal != nullptr) {
    lua_unref(machine, function);
    throw engine::ScriptException(refusal);
  }
  subscriber_->add(std::make_unique<LuauEventSubscription>(
      LuauEventSubscriptionParts{.bus = *bus_,
                                 .handle = subscribeNamed(name, function),
                                 .machine = machine,
                                 .function = function}));
}

void LuauEventBindings::publish(ScriptEvent event) {
  if (bus_ == nullptr) {
    throw engine::ScriptException(EMISSION_OUTSIDE_GAME_MESSAGE);
  }
  if (std::ranges::find(GAME_EVENT_NAMES, event.name) !=
      GAME_EVENT_NAMES.end()) {
    throw engine::ScriptException(EMISSION_OF_GAME_EVENT_MESSAGE);
  }
  bus_->publish(std::move(event));
}

template <typename Event>
engine::SubscriptionHandle LuauEventBindings::subscribeTo(
    LuauEventSubscriptions& subscriber, int function) {
  return bus_->subscribe<Event>(
      [this, &subscriber, function](const Event& event) {
        deliver(subscriber, function, fieldsOf(event));
      });
}

engine::SubscriptionHandle LuauEventBindings::subscribeNamed(
    const std::string& name, int function) {
  LuauEventSubscriptions& subscriber = *subscriber_;
  if (name == ENTITY_DESTROYED_EVENT_NAME) {
    return subscribeTo<EntityDestruction>(subscriber, function);
  }
  if (name == ENTITY_DAMAGED_EVENT_NAME) {
    return subscribeTo<EntityDamage>(subscriber, function);
  }
  if (name == ENEMY_ACTION_STARTED_EVENT_NAME) {
    return subscribeTo<EnemyActionStarted>(subscriber, function);
  }
  if (name == BOSS_HEALTH_CHANGED_EVENT_NAME) {
    return subscribeTo<BossHealthChanged>(subscriber, function);
  }
  return bus_->subscribe<ScriptEvent>(
      [this, heard = name, &subscriber, function](const ScriptEvent& event) {
        if (event.name == heard) {
          deliver(subscriber, function, event.fields);
        }
      });
}

void LuauEventBindings::deliver(LuauEventSubscriptions& subscriber,
                                int function,
                                std::span<const ScriptEventField> fields) {
  engine::ScriptMachine& machine = machine_.get();
  if (!machine.hasTickBudget()) {
    return;
  }
  PendingDelivery pending{.thread = deliveryThread_,
                          .machine = machine.state(),
                          .function = function,
                          .fields = fields};
  try {
    engine::runProtected(
        machine.state(),
        engine::LuauProtectedWork{.run = pushDelivery, .context = &pending});
    if (machine.resume(pending.deliveryThread, 1) ==
        engine::LuauResumeResult::Yielded) {
      failSubscriber(subscriber, WAIT_IN_SUBSCRIBED_FUNCTION_MESSAGE);
    } else {
      lua_settop(pending.deliveryThread, 0);
    }
  } catch (const engine::ScriptException& error) {
    failSubscriber(subscriber, error.what());
  }
}

void LuauEventBindings::failSubscriber(LuauEventSubscriptions& subscriber,
                                       const std::string& reason) {
  deliveryThread_.reset();
  subscriber.fail(SUBSCRIBED_FUNCTION_FAILURE_PREFIX + reason);
}

}  // namespace rtype::game

#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief `listener` keeps the id of the last entity destroyed, and asks for
 * it as its top speed on every tick. */
constexpr std::string_view LISTENER_SCRIPT = R"(
behaviours.register("listener", {}, function(view)
  local heard = nil
  bus.on("entity_destroyed", function(event) heard = event.id end)
  while true do
    behaviours.wait(16, { top_speed = heard })
  end
end)
)";
constexpr std::string_view LISTENER_BEHAVIOUR = "listener";
constexpr std::uint32_t DESTROYED_ENTITY_ID = 42;
constexpr float TOP_SPEED_OF_THE_DESTROYED_ID = 42.0F;

/** @brief `emitter` publishes `alarm` at the level 7 on every tick, and `echo`
 * asks for the level of the last alarm as its top speed. */
constexpr std::string_view ALARM_SCRIPT = R"(
behaviours.register("emitter", {}, function(view)
  bus.emit("alarm", { level = 7 })
  return {}
end)
behaviours.register("echo", {}, function(view)
  local level = nil
  bus.on("alarm", function(event) level = event.level end)
  while true do
    behaviours.wait(16, { top_speed = level })
  end
end)
)";
constexpr std::string_view EMITTER_BEHAVIOUR = "emitter";
constexpr std::string_view ECHO_BEHAVIOUR = "echo";
constexpr float TOP_SPEED_OF_THE_ALARM_LEVEL = 7.0F;

/** @brief An event `echo` did not subscribe to, with a level of its own. */
constexpr std::string_view UNHEARD_EVENT_NAME = "drill";
constexpr const char* LEVEL_KEY = "level";
constexpr double LEVEL_OF_THE_UNHEARD_EVENT = 3.0;

/** @brief `relay` publishes `heard` with its own id each time an entity is
 * damaged; `faulty` fails each time; `sleeper` waits each time. */
constexpr std::string_view DAMAGE_SCRIPT = R"(
behaviours.register("relay", {}, function(view)
  local who = view.id
  bus.on("entity_damaged", function(event) bus.emit("heard", { who = who }) end)
  while true do
    behaviours.wait(1000, {})
  end
end)
behaviours.register("faulty", {}, function(view)
  bus.on("entity_damaged", function(event) error("broken") end)
  while true do
    behaviours.wait(1000, { direction = { x = -1, y = 0 } })
  end
end)
behaviours.register("sleeper", {}, function(view)
  bus.on("entity_damaged", function(event) coroutine.yield() end)
  while true do
    behaviours.wait(1000, {})
  end
end)
)";
constexpr std::string_view RELAY_BEHAVIOUR = "relay";
constexpr std::string_view FAULTY_BEHAVIOUR = "faulty";
constexpr std::string_view SLEEPER_BEHAVIOUR = "sleeper";
constexpr std::string_view HEARD_EVENT_NAME = "heard";
constexpr const char* HEARD_WHO_KEY = "who";
constexpr std::uint32_t FIRST_RELAY_ID = 1;
constexpr std::uint32_t SECOND_RELAY_ID = 2;
constexpr std::uint32_t DAMAGED_ENTITY_ID = 9;
constexpr float FULL_LEFTWARD_OF_THE_FAULTY = -1.0F;

/** @brief `forger` publishes under the name of an event of the game, and
 * `nester` a value that is a table. */
constexpr std::string_view REFUSED_EMISSION_SCRIPT = R"(
behaviours.register("forger", {}, function(view)
  bus.emit("entity_destroyed", { id = 1 })
  return {}
end)
behaviours.register("nester", {}, function(view)
  bus.emit("alarm", { nested = {} })
  return {}
end)
)";
constexpr std::string_view FORGER_BEHAVIOUR = "forger";
constexpr std::string_view NESTER_BEHAVIOUR = "nester";

/** @brief A script that subscribes while it is read, outside any body. */
constexpr std::string_view SUBSCRIPTION_AT_LOAD_SCRIPT = R"(
bus.on("entity_damaged", function(event) end)
)";

/** @brief `eager` subscribes a function on every tick, since its function
 * returns and starts again. */
constexpr std::string_view EAGER_SCRIPT = R"(
behaviours.register("eager", {}, function(view)
  bus.on("entity_damaged", function(event) end)
  return {}
end)
)";
constexpr std::string_view EAGER_BEHAVIOUR = "eager";

/** @brief The functions an entity may keep subscribed, as the scripting page
 * states. */
constexpr std::size_t SUBSCRIPTIONS_AN_ENTITY_MAY_KEEP = 16;

/** @brief `ping` answers each `pong` with a `ping`, after a first one, and
 * `pong` answers each `ping` with a `pong`, without end. */
constexpr std::string_view PING_PONG_SCRIPT = R"(
behaviours.register("ping", {}, function(view)
  bus.on("pong", function(event) bus.emit("ping") end)
  bus.emit("ping")
  while true do
    behaviours.wait(1000, {})
  end
end)
behaviours.register("pong", {}, function(view)
  bus.on("ping", function(event) bus.emit("pong") end)
  while true do
    behaviours.wait(1000, {})
  end
end)
)";
constexpr std::string_view PING_BEHAVIOUR = "ping";
constexpr std::string_view PONG_BEHAVIOUR = "pong";

/** @brief The start of the report of a subscribed function that failed. */
constexpr std::string_view FAILED_SUBSCRIBED_FUNCTION_REPORT =
    "a function subscribed to an event failed";

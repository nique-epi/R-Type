#pragma once

#include <string_view>

/** @brief The behaviour of the scripts of these tests. */
constexpr std::string_view HANDLE_BEHAVIOUR = "handle";

/** @brief A script that counts the steps of its body in its data, and asks
 * for a top speed of that count. */
constexpr std::string_view COUNTING_SCRIPT = R"(
behaviours.register("handle", {}, function(e)
  e.data.count = (e.data.count or 0) + 1
  return { top_speed = e.data.count }
end)
)";
constexpr float AFTER_ONE_STEP = 1.0F;
constexpr float AFTER_TWO_STEPS = 2.0F;

/** @brief A script whose first body keeps its handle in a global, and whose
 * next bodies read the data of that handle. */
constexpr std::string_view DATA_KEEPING_SCRIPT = R"(
behaviours.register("handle", {}, function(e)
  if kept == nil then
    kept = e
    return {}
  end
  local _ = kept.data
  return {}
end)
)";

/** @brief A script whose first body keeps its handle in a global, and whose
 * next bodies read the health of that handle. */
constexpr std::string_view HEALTH_KEEPING_SCRIPT = R"(
behaviours.register("handle", {}, function(e)
  if kept == nil then
    kept = e
    return {}
  end
  local _ = kept.health
  return {}
end)
)";

/** @brief A script that replaces the data of its body. */
constexpr std::string_view DATA_REPLACING_SCRIPT = R"(
behaviours.register("handle", {}, function(e)
  e.data = {}
  return {}
end)
)";

/** @brief A script that fires when the handle a wait returns is the one its
 * function received. */
constexpr std::string_view WAITING_SCRIPT = R"(
behaviours.register("handle", {}, function(e)
  local again = behaviours.wait(0, {})
  return { shooting = again == e }
end)
)";

/** @brief A script that moves its body. */
constexpr std::string_view MOVING_SCRIPT = R"(
behaviours.register("handle", {}, function(e)
  e.position = { x = 10, y = 20 }
  return {}
end)
)";

/** @brief What the failures of those scripts say. */
constexpr std::string_view GONE_BODY_FAILURE = "this body is gone";
constexpr std::string_view OTHER_BODY_FAILURE =
    "the components of a body are only reached during its own step";
constexpr std::string_view OUTSIDE_GAME_FAILURE =
    "the components of a body are only written during a game";
constexpr std::string_view DATA_REPLACED_FAILURE =
    "e.data cannot be replaced: write in it";

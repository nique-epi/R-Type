#pragma once

#include <optional>
#include <string_view>
#include "AppliedHit.hpp"

/** @brief A handler that raises the fields of the hit it is told of, so the
 * message of its failure shows what it read. */
constexpr std::string_view ECHO_SCRIPT = R"(
behaviours.on_hit("echo", function(hit)
  error(string.format("hit %d %d %d %s %s", hit.id, hit.missile, hit.damage,
    tostring(hit.health), tostring(hit.destroyed)))
end)
)";
constexpr std::string_view ECHO_HANDLER = "echo";

/** @brief A hit on Bydo 21 by missile 7 that took 3 points and left 2, and
 * what the echo handler raises of it; the same hit on a body without
 * Health, which took nothing and left nothing. */
constexpr rtype::game::AppliedHit HIT_THAT_LEFT_TWO{.targetNetworkId = 21,
                                                    .missileNetworkId = 7,
                                                    .damage = 3,
                                                    .healthLeft = 2,
                                                    .destroyed = false};
constexpr std::string_view ECHO_OF_THE_HIT_THAT_LEFT_TWO = "hit 21 7 3 2 false";
constexpr rtype::game::AppliedHit HIT_ON_A_STEEL_WALL{
    .targetNetworkId = 21,
    .missileNetworkId = 7,
    .damage = 0,
    .healthLeft = std::nullopt,
    .destroyed = false};
constexpr std::string_view ECHO_OF_THE_HIT_ON_A_STEEL_WALL =
    "hit 21 7 0 nil false";

/** @brief A handler that always fails, one that waits, and two handlers of
 * one name. */
constexpr std::string_view CRACKING_SCRIPT = R"(
behaviours.on_hit("crack", function(hit) error("cracked") end)
behaviours.register("stuck", {}, function(e)
  while true do
  end
end)
)";
constexpr std::string_view CRACK_HANDLER = "crack";
constexpr std::string_view FAILURE_OF_THE_CRACK_HANDLER =
    "the on_hit function crack failed: ";
constexpr std::string_view WAITING_SCRIPT = R"(
behaviours.on_hit("slow", function(hit) coroutine.yield() end)
)";
constexpr std::string_view SLOW_HANDLER = "slow";
constexpr std::string_view WAIT_REFUSAL = "an on_hit function may not wait";
constexpr std::string_view TWIN_SCRIPT = R"(
behaviours.on_hit("twin", function(hit) end)
behaviours.on_hit("twin", function(hit) end)
)";

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "BehaviourRegistry.hpp"
#include "BehaviourScripts.hpp"
#include "BehaviourView.hpp"
#include "Intent.hpp"
#include "Position.hpp"
#include "ScriptFailureReport.hpp"
#include "ScriptSource.hpp"

/** @brief The failures the scripted behaviours of a test reported. */
struct FailureLog {
  std::vector<std::string> messages;
};

/** @returns A report that writes into @p log, which must outlive it. */
[[nodiscard]] rtype::game::ScriptFailureReport reportInto(FailureLog& log);

/** @returns The scripts of a test: @p text, named TEST_SCRIPT_NAME. */
[[nodiscard]] std::vector<rtype::engine::ScriptSource> scriptsOf(
    std::string_view text);

/** @returns A view of a body at @p position, at the settings
 * HALF_SPEED_SETTINGS, with the ship @p nearestShip, and wounded. */
[[nodiscard]] rtype::game::BehaviourView woundedViewAt(
    rtype::game::Position position, rtype::game::Position nearestShip);

/** @returns What the behaviour @p name of @p behaviours wants for @p view. */
[[nodiscard]] rtype::game::Intent steeringOf(
    const rtype::game::BehaviourRegistry& behaviours, std::string_view name,
    const rtype::game::BehaviourView& view);

/** @returns The view woundedViewAt() gives, with @p healthShare of its health
 * left. */
[[nodiscard]] rtype::game::BehaviourView viewWithHealth(float healthShare);

/** @returns The behaviours of the example resource the repository ships. */
[[nodiscard]] rtype::game::BehaviourRegistry exampleBehaviours(
    rtype::game::BehaviourScripts& scripts);

/** @returns A view of a body in the middle at the settings of the example
 * sheet, with the ship @p nearestShip. */
[[nodiscard]] rtype::game::BehaviourView exampleViewWith(
    rtype::game::Position nearestShip);

#include "BehaviourScriptsFixtures.hpp"
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "BehaviourRegistry.hpp"
#include "BehaviourScriptsTestConstants.hpp"
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"
#include "Position.hpp"
#include "ScriptFailureReport.hpp"
#include "ScriptSource.hpp"

rtype::game::ScriptFailureReport reportInto(FailureLog& log) {
  return
      [&log](const std::string& message) { log.messages.push_back(message); };
}

std::vector<rtype::engine::ScriptSource> scriptsOf(std::string_view text) {
  return {rtype::engine::ScriptSource{.name = std::string{TEST_SCRIPT_NAME},
                                      .text = std::string{text}}};
}

rtype::game::BehaviourView woundedViewAt(rtype::game::Position position,
                                         rtype::game::Position nearestShip) {
  return rtype::game::BehaviourView{.position = position,
                                    .origin = position,
                                    .age = {},
                                    .elapsed = {},
                                    .topSpeed = 0.0F,
                                    .settings = HALF_SPEED_SETTINGS,
                                    .action = std::nullopt,
                                    .actionAge = {},
                                    .nearestShip = nearestShip,
                                    .wounded = true};
}

rtype::game::Intent steeringOf(const rtype::game::BehaviourRegistry& behaviours,
                               std::string_view name,
                               const rtype::game::BehaviourView& view) {
  const rtype::game::IBehaviour* const behaviour = behaviours.find(name);
  return behaviour == nullptr ? rtype::game::Intent{} : behaviour->steer(view);
}

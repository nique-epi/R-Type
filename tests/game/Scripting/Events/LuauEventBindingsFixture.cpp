#include "LuauEventBindingsFixture.hpp"
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "BehaviourScriptsFixtures.hpp"
#include "BehaviourScriptsTestConstants.hpp"
#include "BehaviourView.hpp"
#include "EventBus.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"
#include "LuauBehaviourFixture.hpp"
#include "LuauEventBindings.hpp"
#include "LuauEventBindingsTestConstants.hpp"
#include "ScriptEvent.hpp"
#include "ScriptEventField.hpp"
#include "ScriptMachine.hpp"

LuauEventBindingsFixture::LuauEventBindingsFixture()
    : scripts_(reportInto(log_)) {
  scripts_.eventBindings().attach(bus_);
  static_cast<void>(bus_.subscribe<rtype::game::ScriptEvent>(
      [this](const rtype::game::ScriptEvent& event) {
        published_.push_back(event);
      }));
}

void LuauEventBindingsFixture::load(std::string_view script) {
  scripts_.run(scriptsOf(script));
  behaviours_ = scripts_.behaviours();
}

rtype::game::Intent LuauEventBindingsFixture::steer(std::string_view behaviour,
                                                    SteeredBody& body) {
  scripts_.machine().beginTick();
  rtype::game::BehaviourView view = woundedViewAt(body.position, SHIP_BELOW);
  view.behaviourState = &body.behaviourState;
  view.networkId = body.networkId;
  const rtype::game::IBehaviour* const found = behaviours_.find(behaviour);
  return found == nullptr ? rtype::game::Intent{} : found->steer(view);
}

rtype::engine::EventBus& LuauEventBindingsFixture::bus() { return bus_; }

bool LuauEventBindingsFixture::scriptsHaveBudget() {
  return scripts_.machine().hasTickBudget();
}

const std::vector<rtype::game::ScriptEvent>&
LuauEventBindingsFixture::published() const {
  return published_;
}

rtype::game::ScriptEvent LuauEventBindingsFixture::heardBy(
    std::uint32_t networkId) {
  return rtype::game::ScriptEvent{
      .name = std::string{HEARD_EVENT_NAME},
      .fields = {rtype::game::ScriptEventField{
          .key = HEARD_WHO_KEY, .value = static_cast<double>(networkId)}}};
}

const FailureLog& LuauEventBindingsFixture::failures() const { return log_; }

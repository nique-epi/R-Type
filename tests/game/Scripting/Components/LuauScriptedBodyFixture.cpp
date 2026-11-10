#include "LuauScriptedBodyFixture.hpp"
#include <string_view>
#include "BehaviourScriptsFixtures.hpp"
#include "BehaviourScriptsTestConstants.hpp"
#include "BehaviourView.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"
#include "LuauEntityHandleTestConstants.hpp"
#include "Position.hpp"

LuauScriptedBodyFixture::LuauScriptedBodyFixture()
    : scripts_(reportInto(log_)),
      components_(entities_),
      orders_(FIRST_ORDERED_ID) {
  scripts_.attachGame(orders_, components_);
}

void LuauScriptedBodyFixture::load(std::string_view script) {
  scripts_.run(scriptsOf(script));
  behaviours_ = scripts_.behaviours();
}

rtype::engine::Entity LuauScriptedBodyFixture::addBody() {
  return entities_.create();
}

rtype::engine::ComponentRegistry& LuauScriptedBodyFixture::components() {
  return components_;
}

rtype::game::Intent LuauScriptedBodyFixture::steer(std::string_view behaviour,
                                                   rtype::engine::Entity body) {
  scripts_.machine().beginTick();
  rtype::game::BehaviourView view =
      woundedViewAt(rtype::game::Position{}, SHIP_BELOW);
  view.behaviourState = &behaviourStates_[body.index];
  view.entity = body;
  const rtype::game::IBehaviour* const found = behaviours_.find(behaviour);
  return found == nullptr ? rtype::game::Intent{} : found->steer(view);
}

void LuauScriptedBodyFixture::endBody(rtype::engine::Entity body) {
  behaviourStates_.erase(body.index);
}

const FailureLog& LuauScriptedBodyFixture::failures() const { return log_; }

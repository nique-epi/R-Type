#include "LuauBehaviourFixture.hpp"
#include <cstddef>
#include <string_view>
#include "BehaviourScriptsFixtures.hpp"
#include "BehaviourScriptsTestConstants.hpp"
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "IEntityOrders.hpp"
#include "Intent.hpp"
#include "lua.h"

LuauBehaviourFixture::LuauBehaviourFixture()
    : scripts_(reportInto(log_)), components_(entities_) {}

void LuauBehaviourFixture::load(std::string_view script) {
  scripts_.run(scriptsOf(script));
  behaviours_ = scripts_.behaviours();
}

rtype::game::IHitHandler* LuauBehaviourFixture::hitHandler(
    std::string_view name) {
  return behaviours_.findHitHandler(name);
}

void LuauBehaviourFixture::attach(rtype::game::IEntityOrders& orders) {
  scripts_.attachGame(orders, components_);
}

void LuauBehaviourFixture::beginTick() { scripts_.machine().beginTick(); }

rtype::game::Intent LuauBehaviourFixture::steer(std::string_view behaviour,
                                                SteeredBody& body) {
  rtype::game::BehaviourView view = woundedViewAt(body.position, SHIP_BELOW);
  view.behaviourState = &body.behaviourState;
  view.networkId = body.networkId;
  const rtype::game::IBehaviour* const found = behaviours_.find(behaviour);
  return found == nullptr ? rtype::game::Intent{} : found->steer(view);
}

std::size_t LuauBehaviourFixture::memoryAfterCollection() {
  lua_gc(scripts_.machine().state(), LUA_GCCOLLECT, 0);
  return scripts_.machine().memoryUsed();
}

const FailureLog& LuauBehaviourFixture::failures() const { return log_; }

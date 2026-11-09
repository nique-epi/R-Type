#include "BehaviourRoutineFixture.hpp"
#include <cstddef>
#include <string_view>
#include "BehaviourScriptsFixtures.hpp"
#include "BehaviourScriptsTestConstants.hpp"
#include "BehaviourView.hpp"
#include "IBehaviour.hpp"
#include "IScriptWorld.hpp"
#include "Intent.hpp"
#include "lua.h"

BehaviourRoutineFixture::BehaviourRoutineFixture()
    : scripts_(reportInto(log_)) {}

void BehaviourRoutineFixture::load(std::string_view script) {
  scripts_.run(scriptsOf(script));
  behaviours_ = scripts_.behaviours();
}

void BehaviourRoutineFixture::attach(rtype::game::IScriptWorld& world) {
  scripts_.attachWorld(world);
}

void BehaviourRoutineFixture::beginTick() { scripts_.machine().beginTick(); }

rtype::game::Intent BehaviourRoutineFixture::steer(std::string_view behaviour,
                                                   SteeredBody& body) {
  rtype::game::BehaviourView view = woundedViewAt(body.position, SHIP_BELOW);
  view.memory = &body.memory;
  const rtype::game::IBehaviour* const found = behaviours_.find(behaviour);
  return found == nullptr ? rtype::game::Intent{} : found->steer(view);
}

std::size_t BehaviourRoutineFixture::memoryAfterCollection() {
  lua_gc(scripts_.machine().state(), LUA_GCCOLLECT, 0);
  return scripts_.machine().memoryUsed();
}

const FailureLog& BehaviourRoutineFixture::failures() const { return log_; }

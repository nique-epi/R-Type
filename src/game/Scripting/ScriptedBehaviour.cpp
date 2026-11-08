#include "ScriptedBehaviour.hpp"
#include <cstddef>
#include <span>
#include <string_view>
#include <utility>
#include <vector>
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "EngineException.hpp"
#include "Intent.hpp"
#include "ScriptMachine.hpp"
#include "ScriptView.hpp"
#include "ScriptedBehaviourParts.hpp"
#include "ScriptedSetting.hpp"
#include "lua.h"

namespace rtype::game {

ScriptedBehaviour::ScriptedBehaviour(ScriptedBehaviourParts parts)
    : parts_(std::move(parts)) {
  settingNames_.reserve(parts_.settings.size());
  settings_.reserve(parts_.settings.size());
  for (const ScriptedSetting& setting : parts_.settings) {
    settingNames_.push_back(setting.name);
  }
  for (std::size_t index = 0; index < parts_.settings.size(); ++index) {
    settings_.push_back(BehaviourSetting{
        .name = settingNames_[index], .range = parts_.settings[index].range});
  }
}

std::string_view ScriptedBehaviour::name() const { return parts_.name; }

std::span<const BehaviourSetting> ScriptedBehaviour::settings() const {
  return settings_;
}

Intent ScriptedBehaviour::steer(const BehaviourView& view) const {
  try {
    return steerByScript(view);
  } catch (const engine::ScriptException& error) {
    if (!failureReported_) {
      failureReported_ = true;
      parts_.reportFailure("the behaviour " + parts_.name +
                           " failed: " + error.what());
    }
    return Intent{.steered = true};
  }
}

Intent ScriptedBehaviour::steerByScript(const BehaviourView& view) const {
  engine::ScriptMachine& machine = parts_.machine.get();
  lua_State* const state = machine.state();
  const int top = lua_gettop(state);
  lua_getref(state, parts_.steer);
  pushBehaviourView(state, view, settingNames_);
  machine.call(1, 1);
  const Intent intent = intentAt(state, -1);
  lua_settop(state, top);
  return intent;
}

}  // namespace rtype::game

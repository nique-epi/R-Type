#pragma once

#include <functional>
#include <string>
#include <vector>
#include "LuauEventBindings.hpp"
#include "LuauResumeContext.hpp"
#include "LuauSetting.hpp"
#include "ScriptFailureReport.hpp"
#include "ScriptMachine.hpp"

namespace rtype::game {

/** @brief What a Luau behaviour is made of: the machine its script runs
 * on, its name, its settings, the reference of its steering function in the
 * registry of the machine, where it reports a failure, the state of the
 * step in progress its script shares with the functions it calls, and what
 * is told which entity steps, so that `bus.on` subscribes for it. */
struct LuauBehaviourParts {
  std::reference_wrapper<engine::ScriptMachine> machine;
  std::string name;
  std::vector<LuauSetting> settings;
  int steer{0};
  ScriptFailureReport reportFailure;
  std::reference_wrapper<LuauResumeContext> resumeContext;
  std::reference_wrapper<LuauEventBindings> eventBindings;
};

}  // namespace rtype::game

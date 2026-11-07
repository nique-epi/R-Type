#pragma once

#include <functional>
#include <string>
#include <vector>
#include "ScriptFailureReport.hpp"
#include "ScriptMachine.hpp"
#include "ScriptedSetting.hpp"

namespace rtype::game {

/** @brief What a scripted behaviour is made of: the machine its script runs
 * on, its name, its settings, the reference of its steering function in the
 * registry of the machine, and where it reports a failure. */
struct ScriptedBehaviourParts {
  std::reference_wrapper<engine::ScriptMachine> machine;
  std::string name;
  std::vector<ScriptedSetting> settings;
  int steer{0};
  ScriptFailureReport reportFailure;
};

}  // namespace rtype::game

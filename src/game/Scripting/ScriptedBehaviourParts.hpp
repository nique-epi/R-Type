#pragma once

#include <functional>
#include <string>
#include <vector>
#include "ScriptBodyRemoval.hpp"
#include "ScriptFailureReport.hpp"
#include "ScriptMachine.hpp"
#include "ScriptWaitState.hpp"
#include "ScriptedSetting.hpp"

namespace rtype::game {

/** @brief What a scripted behaviour is made of: the machine its script runs
 * on, its name, its settings, the reference of its steering function in the
 * registry of the machine, where it reports a failure, and how it removes a
 * body whose script failed. */
struct ScriptedBehaviourParts {
  std::reference_wrapper<engine::ScriptMachine> machine;
  std::string name;
  std::vector<ScriptedSetting> settings;
  int steer{0};
  ScriptFailureReport reportFailure;
  std::reference_wrapper<ScriptWaitState> waits;
  ScriptBodyRemoval removeBody;
};

}  // namespace rtype::game

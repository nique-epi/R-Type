#pragma once

#include <functional>
#include "LuauResumeContext.hpp"
#include "ScriptFailureReport.hpp"
#include "ScriptMachine.hpp"

namespace rtype::game {

/** @brief What LuauEventBindings is built from: the machine its table is
 * exposed on, what says which entity steps, and where the first failure of a
 * subscribed function is reported. The first two must outlive it. */
struct LuauEventBindingsParts {
  std::reference_wrapper<engine::ScriptMachine> machine;
  std::reference_wrapper<LuauResumeContext> resumeContext;
  ScriptFailureReport reportFailure;
};

}  // namespace rtype::game

#pragma once

#include <functional>
#include <string>
#include "ScriptFailureReport.hpp"
#include "ScriptMachine.hpp"

namespace rtype::game {

/**
 * @brief What a LuauHitHandler is made of: the machine its function lives on,
 * the name a sheet gives it, the reference of its function in the registry of
 * the machine, which it takes over, and where its failure is reported.
 *
 * A std::reference_wrapper has no default constructor, so this struct is only
 * ever built with designated initializers naming every member.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct LuauHitHandlerParts {
  std::reference_wrapper<engine::ScriptMachine> machine;
  std::string name;
  int function{0};
  ScriptFailureReport reportFailure;
};

}  // namespace rtype::game

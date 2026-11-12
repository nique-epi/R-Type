#pragma once

#include <string_view>
#include "AppliedHit.hpp"
#include "IHitHandler.hpp"
#include "LuauHitHandlerParts.hpp"

namespace rtype::game {

/**
 * @brief The function a script declared with `behaviours.on_hit(name,
 * function)`: told of each hit applied to a body whose sheet names it, with
 * the table of the hit (see pushAppliedHitTable()).
 *
 * Each call runs in a Luau thread of its own, with the budget of a step,
 * counted in the budget of the tick; once that budget is spent, no call is
 * made. It may not wait. Its first failure is reported once, with its name;
 * the hit stays applied, and later hits still call it: a handler serves every
 * body of a sheet, not one body. Neither copied nor moved: its destructor
 * gives its reference back, and must run while the machine is alive, as the
 * behaviours of a BehaviourScripts.
 */
class LuauHitHandler final : public IHitHandler {
 public:
  explicit LuauHitHandler(LuauHitHandlerParts parts);
  LuauHitHandler(const LuauHitHandler&) = delete;
  LuauHitHandler& operator=(const LuauHitHandler&) = delete;
  LuauHitHandler(LuauHitHandler&&) = delete;
  LuauHitHandler& operator=(LuauHitHandler&&) = delete;
  ~LuauHitHandler() override;

  [[nodiscard]] std::string_view name() const override;
  void onHit(const AppliedHit& hit) override;

 private:
  /** @brief Calls the function with the table of @p hit.
   * @throws engine::ScriptException when it fails, waits or runs past its
   * budget. */
  void call(const AppliedHit& hit);
  /** @brief Reports @p reason as the failure of the function, the first time
   * only. */
  void reportOnce(std::string_view reason);

  LuauHitHandlerParts parts_;
  bool failureReported_{false};
};

}  // namespace rtype::game

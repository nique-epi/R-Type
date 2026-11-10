#pragma once

#include <memory>
#include <span>
#include <vector>
#include "BehaviourRegistry.hpp"
#include "IBodyOrders.hpp"
#include "LuauBehaviour.hpp"
#include "LuauBehaviourParts.hpp"
#include "LuauStepState.hpp"
#include "ScriptFailureReport.hpp"
#include "ScriptMachine.hpp"
#include "ScriptSource.hpp"

namespace rtype::game {

/**
 * @brief The behaviours the server scripts of a game register: it runs the
 * scripts on a machine of its own, which exposes `behaviours.register(name,
 * settings, steer)`, `behaviours.wait(ms, intent)`, `behaviours.spawn(type,
 * position)` and `behaviours.destroy(id)`, and hands the behaviours over with
 * the built-in ones.
 *
 * It must outlive the behaviours it hands over: each calls its script on its
 * machine. Neither copied nor moved: the machine reaches it by its address.
 */
class BehaviourScripts {
 public:
  /** @param reportFailure Told once for each behaviour whose script fails. */
  explicit BehaviourScripts(ScriptFailureReport reportFailure);
  BehaviourScripts(const BehaviourScripts&) = delete;
  BehaviourScripts& operator=(const BehaviourScripts&) = delete;
  BehaviourScripts(BehaviourScripts&&) = delete;
  BehaviourScripts& operator=(BehaviourScripts&&) = delete;
  ~BehaviourScripts() = default;

  /**
   * @brief Runs each of @p sources once, in their order.
   * @throws engine::ScriptException naming the script and the line when one
   * does not compile, fails or calls `register` wrongly.
   */
  void run(std::span<const engine::ScriptSource> sources);

  /** @brief Keeps a behaviour a script registered: its machine and its report
   * are those of this object. */
  void declare(LuauBehaviourParts parts);

  /** @returns The machine the scripts run on. */
  [[nodiscard]] engine::ScriptMachine& machine();

  /** @brief Makes @p orders carry out the bodies the scripts order. */
  void attachOrders(IBodyOrders& orders);

  /** @brief Forgets @p orders, unless others took their place. */
  void detachOrders(const IBodyOrders& orders) noexcept;

  /** @returns What carries out the bodies the scripts order; nullptr outside
   * a game. */
  [[nodiscard]] IBodyOrders* orders() const;

  /** @returns What the functions the scripts call know of the step in
   * progress. */
  [[nodiscard]] LuauStepState& step();

  /** @returns What the report of a behaviour calls. */
  [[nodiscard]] const ScriptFailureReport& reportFailure() const;

  /**
   * @returns The built-in behaviours and those the scripts registered, which
   * leave this object.
   * @throws DuplicateBehaviourException when a script registered a name a
   * behaviour already has.
   */
  [[nodiscard]] BehaviourRegistry behaviours();

 private:
  engine::ScriptMachine machine_;
  ScriptFailureReport reportFailure_;
  LuauStepState step_;
  std::vector<std::unique_ptr<LuauBehaviour>> declared_;
};

}  // namespace rtype::game

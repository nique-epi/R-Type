#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "LuauEventSubscription.hpp"

namespace rtype::game {

/**
 * @brief The functions the script of one entity subscribed to events, which
 * end with this object, and why they ended when one of them failed.
 *
 * It must be destroyed, cleared or failed while no script runs. Neither
 * copied nor moved: the subscriptions of the bus point to it.
 */
class LuauEventSubscriptions {
 public:
  LuauEventSubscriptions() = default;
  LuauEventSubscriptions(const LuauEventSubscriptions&) = delete;
  LuauEventSubscriptions& operator=(const LuauEventSubscriptions&) = delete;
  LuauEventSubscriptions(LuauEventSubscriptions&&) = delete;
  LuauEventSubscriptions& operator=(LuauEventSubscriptions&&) = delete;
  ~LuauEventSubscriptions() = default;

  /** @returns Whether the entity keeps MAXIMUM_SUBSCRIPTIONS_PER_ENTITY
   * functions, and may subscribe no more. */
  [[nodiscard]] bool full() const;

  /** @brief Keeps @p subscription until the entity or its script ends. */
  void add(std::unique_ptr<LuauEventSubscription> subscription);

  /** @brief Ends every subscription, one of their functions having failed
   * for @p reason, which failure() gives until clear(). */
  void fail(const std::string& reason);

  /** @returns Why a subscribed function failed; nothing while none did. */
  [[nodiscard]] const std::optional<std::string>& failure() const;

  /** @brief Ends every subscription and forgets a failure. */
  void clear();

 private:
  std::vector<std::unique_ptr<LuauEventSubscription>> subscriptions_;
  std::optional<std::string> failure_;
};

}  // namespace rtype::game

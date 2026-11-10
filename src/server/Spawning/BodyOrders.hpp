#pragma once

#include <cstdint>
#include <string_view>
#include <vector>
#include "BodyOrder.hpp"
#include "BodyOrdersParts.hpp"
#include "IBodyOrders.hpp"
#include "Logger.hpp"
#include "Position.hpp"

namespace rtype::server {

/**
 * @brief Carries out the orders to create and remove bodies the scripts of a
 * game give, in its world.
 *
 * Bodies cannot be created while PilotSystem visits them, so spawn() and
 * destroy() only take note: the body gets its network id at once, so a script
 * can name it, and onTick() makes it appear, tells every connected player, and
 * then removes the bodies asked for, in the same tick, with an
 * EntityDestruction Removed. A body appears before the removals, so a script
 * can remove in a tick the body it created in it.
 *
 * The first tick whose scripts use the whole of their budget is written in the
 * journal, once per world: bodies then wait for the next tick to step.
 *
 * It works on one world, and attaches itself to the scripts for as long as it
 * lives: the game builds a new one with each new world. Not thread-safe: use
 * it from the simulation thread only.
 */
class BodyOrders final : public game::IBodyOrders {
 public:
  explicit BodyOrders(BodyOrdersParts parts);
  BodyOrders(const BodyOrders&) = delete;
  BodyOrders& operator=(const BodyOrders&) = delete;
  BodyOrders(BodyOrders&&) = delete;
  BodyOrders& operator=(BodyOrders&&) = delete;
  ~BodyOrders() override;

  /** @brief Carries out what the scripts asked since the last call, then
   * gives the scripts the budget of the next tick. */
  void onTick();

  [[nodiscard]] std::uint32_t spawn(std::string_view type,
                                    game::Position position) override;
  void destroy(std::uint32_t networkId) override;

 private:
  void warnWhenOverBudget();
  void appear(const BodyOrder& order) const;
  void removeAsked();

  BodyOrdersParts parts_;
  logging::Logger logger_;
  bool budgetWarned_{false};
  std::vector<BodyOrder> orders_;
  std::vector<std::uint32_t> removals_;
};

}  // namespace rtype::server

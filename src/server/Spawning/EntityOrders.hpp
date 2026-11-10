#pragma once

#include <cstdint>
#include <string_view>
#include <vector>
#include "EntityOrder.hpp"
#include "EntityOrdersParts.hpp"
#include "IEntityOrders.hpp"
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
 * It works on one world, and attaches itself and the bus of the game to the
 * scripts for as long as it lives: the game builds a new one with each new
 * world. Not thread-safe: use it from the simulation thread only.
 */
class EntityOrders final : public game::IEntityOrders {
 public:
  explicit EntityOrders(EntityOrdersParts parts);
  EntityOrders(const EntityOrders&) = delete;
  EntityOrders& operator=(const EntityOrders&) = delete;
  EntityOrders(EntityOrders&&) = delete;
  EntityOrders& operator=(EntityOrders&&) = delete;
  ~EntityOrders() override;

  /** @brief Carries out what the scripts asked since the last call, then
   * gives the scripts the budget of the next tick. */
  void onTick();

  [[nodiscard]] std::uint32_t spawn(std::string_view type,
                                    game::Position position) override;
  void destroy(std::uint32_t networkId) override;

 private:
  void warnWhenOverBudget();
  void appear(const EntityOrder& order) const;
  void removeAsked();

  EntityOrdersParts parts_;
  logging::Logger logger_;
  bool budgetWarned_{false};
  std::vector<EntityOrder> orders_;
  std::vector<std::uint32_t> removals_;
};

}  // namespace rtype::server

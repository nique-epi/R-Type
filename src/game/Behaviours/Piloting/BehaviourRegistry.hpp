#pragma once

#include <memory>
#include <string_view>
#include <vector>
#include "IBehaviour.hpp"
#include "IHitHandler.hpp"

namespace rtype::game {

/**
 * @brief The behaviours a body may be given, and the hit handlers its sheet
 * may name in `on_hit`, found by their name.
 *
 * It owns them, each behind its own pointer, so a Pilot may keep a reference
 * to one for as long as the registry lives, wherever the registry is moved. A
 * behaviour is looked up when a body appears or a sheet is read, never during
 * a tick.
 */
class BehaviourRegistry {
 public:
  /**
   * @brief Adds @p behaviour under its name().
   * @throws DuplicateBehaviourException When a behaviour of that name is
   * already in the registry.
   */
  void add(std::unique_ptr<IBehaviour> behaviour);

  /** @returns The behaviour named @p name, or nullptr when none is: the reader
   * of a sheet asks it, so its error names the file and the key. */
  [[nodiscard]] const IBehaviour* find(std::string_view name) const;

  /** @returns The names of the behaviours, in the order they were added. */
  [[nodiscard]] std::vector<std::string_view> names() const;

  /**
   * @brief Adds @p handler under its name(): a sheet may name it in `on_hit`.
   * @throws DuplicateHitHandlerException When a handler of that name is
   * already in the registry.
   */
  void addHitHandler(std::unique_ptr<IHitHandler> handler);

  /** @returns The hit handler named @p name, or nullptr when none is. */
  [[nodiscard]] IHitHandler* findHitHandler(std::string_view name) const;

  /** @returns The names of the hit handlers, in the order they were added. */
  [[nodiscard]] std::vector<std::string_view> hitHandlerNames() const;

 private:
  std::vector<std::unique_ptr<IBehaviour>> behaviours_;
  std::vector<std::unique_ptr<IHitHandler>> hitHandlers_;
};

}  // namespace rtype::game

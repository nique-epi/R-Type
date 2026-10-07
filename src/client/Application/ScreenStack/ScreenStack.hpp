#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include "IScreen.hpp"

namespace rtype::client {

/** @brief Builds a screen when the stack applies the change that needs it; a
 * factory that builds nothing changes nothing. */
using ScreenFactory = std::function<std::unique_ptr<IScreen>()>;

/**
 * @brief The screens of the client, the active one on top.
 *
 * push(), pop() and replaceAll() only ask for a change; applyPendingChanges()
 * applies them, in the order they were asked for. A screen may therefore ask
 * to leave the stack while it handles an event, and stays alive until that
 * call. A screen is built only when its change is applied, once the screens
 * that change removes are destroyed.
 */
class ScreenStack {
 public:
  /** @brief Asks for a screen to be put over the others, which stay below
   * it. */
  void push(ScreenFactory factory);

  /** @brief Asks for the screen on top to be removed; nothing happens if the
   * stack is empty by then. */
  void pop();

  /** @brief Asks for every screen to be destroyed, from the top down, then for
   * the new one to be built as the only one. */
  void replaceAll(ScreenFactory factory);

  /** @brief Applies the changes asked for so far; the changes asked for while
   * they are applied wait for the next call. */
  void applyPendingChanges();

  /** @brief Gives the event to the screen on top only. */
  void handleEvent(const sf::Event& event);

  /** @brief Updates every screen, from the bottom up. */
  void update();

  /** @brief Draws every screen, from the bottom up. */
  void draw(sf::RenderTarget& target) const;

  [[nodiscard]] bool isEmpty() const;

 private:
  enum class ChangeKind : std::uint8_t {
    Push,
    Pop,
    ReplaceAll,
  };

  struct Change {
    ChangeKind kind{};
    ScreenFactory factory;
  };

  void apply(const Change& change);
  void build(const ScreenFactory& factory);

  std::vector<std::unique_ptr<IScreen>> screens_;
  std::vector<Change> pendingChanges_;
};

}  // namespace rtype::client

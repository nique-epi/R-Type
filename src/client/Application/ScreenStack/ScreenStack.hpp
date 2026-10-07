#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include "IScreen.hpp"

namespace rtype::client {

/** @brief Builds a screen when the stack applies the change that needs it. */
using ScreenFactory = std::function<std::unique_ptr<IScreen>()>;

/** @brief The screens of the client, the active one on top, changed only when
 * the asked changes are applied. */
class ScreenStack {
 public:
  ScreenStack() = default;
  ~ScreenStack() = default;

  ScreenStack(const ScreenStack&) = delete;
  ScreenStack& operator=(const ScreenStack&) = delete;
  ScreenStack(ScreenStack&&) = delete;
  ScreenStack& operator=(ScreenStack&&) = delete;

  /** @brief Asks for a screen to be put over the others. */
  void push(ScreenFactory factory);

  /** @brief Asks for the screen on top to be removed. */
  void pop();

  /** @brief Asks for every screen to be destroyed, top first, before the new
   * one is built. */
  void replaceAll(ScreenFactory factory);

  /** @brief Applies the changes asked for so far, in order; those asked
   * meanwhile wait for the next call. */
  void applyPendingChanges();

  /** @brief Gives the event to the screen on top. */
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

  std::vector<Change> pendingChanges_;
  std::vector<std::unique_ptr<IScreen>> screens_;
};

}  // namespace rtype::client

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ScreenId.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief Builds the screen of an identifier; returning nothing adds no
 * screen. */
using ScreenFactory = std::function<std::unique_ptr<IScreen>(ScreenId)>;

/**
 * @brief The screens of the client, the active one on top.
 *
 * Screens are asked for by identifier: push(), pop(), replaceAll() and
 * closeAll() only ask for a change; applyPendingChanges() applies them, in the
 * order they were asked for. A screen may therefore ask to leave the stack
 * while it handles an event, and stays alive until that call. A screen is built
 * only when its change is applied, once the screens that change removes are
 * destroyed.
 *
 * Screens see the stack only as an IScreenNavigator: they can ask for
 * changes, while applying them, updating, drawing and handing out events stay
 * with the frame loop. They keep a pointer to it, so it can be neither copied
 * nor moved.
 */
class ScreenStack : public IScreenNavigator {
 public:
  /** @param factory Builds every screen the stack is asked for. */
  explicit ScreenStack(ScreenFactory factory);
  ~ScreenStack() override = default;

  ScreenStack(const ScreenStack&) = delete;
  ScreenStack& operator=(const ScreenStack&) = delete;
  ScreenStack(ScreenStack&&) = delete;
  ScreenStack& operator=(ScreenStack&&) = delete;

  void push(ScreenId screen) override;
  void pop() override;
  void replaceAll(ScreenId screen) override;
  void closeAll() override;

  /** @brief Applies the changes asked for so far; the changes asked for while
   * they are applied wait for the next call. */
  void applyPendingChanges();

  /** @brief Gives the event to the screen on top only. */
  void handleEvent(const sf::Event& event);

  /** @brief Updates every screen, from the bottom up, by the time since the
   * previous frame. */
  void update(engine::Duration elapsed);

  /** @brief Draws every screen, from the bottom up. */
  void draw(sf::RenderTarget& target) const;

  [[nodiscard]] bool isEmpty() const;

 private:
  enum class ChangeKind : std::uint8_t {
    Push,
    Pop,
    ReplaceAll,
    CloseAll,
  };

  struct Change {
    ChangeKind kind{};
    ScreenId screen{};
  };

  void apply(const Change& change);
  void build(ScreenId screen);
  void destroyAll();

  ScreenFactory factory_;
  std::vector<Change> pendingChanges_;
  std::vector<std::unique_ptr<IScreen>> screens_;
};

}  // namespace rtype::client

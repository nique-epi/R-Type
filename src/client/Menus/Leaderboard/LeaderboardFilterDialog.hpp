#pragma once

#include <SFML/Window/Event.hpp>
#include <functional>
#include <optional>
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "LeaderboardFilterChoice.hpp"
#include "Menu.hpp"
#include "PlayfieldMapping.hpp"
#include "TextMetrics.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The dialog over the leaderboard that picks a difficulty and a number
 * of pilots, and takes every event until it is closed. Nothing is applied
 * before APPLY: its rows change a copy of the choice.
 *
 * Its buttons and its rows hold a pointer to it, so it can be neither copied
 * nor moved.
 */
class LeaderboardFilterDialog {
 public:
  /** @param sounds Plays the click of the dialog; it must outlive the dialog.
   */
  explicit LeaderboardFilterDialog(ISoundPlayer& sounds);

  LeaderboardFilterDialog(const LeaderboardFilterDialog&) = delete;
  LeaderboardFilterDialog& operator=(const LeaderboardFilterDialog&) = delete;
  LeaderboardFilterDialog(LeaderboardFilterDialog&&) = delete;
  LeaderboardFilterDialog& operator=(LeaderboardFilterDialog&&) = delete;
  ~LeaderboardFilterDialog() = default;

  /** @brief Opens the dialog on @p choice, laid out at the text size of
   * @p metrics; APPLY runs @p apply with what the player picked and closes
   * it; BACK closes it. */
  void open(LeaderboardFilterChoice choice,
            const std::function<void(LeaderboardFilterChoice)>& apply,
            TextMetrics metrics = {});

  [[nodiscard]] bool isOpen() const;

  /** @brief Gives @p event to the dialog, which must be open. */
  void handleEvent(const sf::Event& event, const PlayfieldMapping& pointAt);

  /** @brief Lets @p elapsed pass for the dialog, when it is open. */
  void advance(engine::Duration elapsed);

  /** @brief Paints the dialog when it is open. */
  void paint(IInterfaceCanvas& canvas) const;

 private:
  /** @brief Closes the dialog once one of its buttons has run: an action must
   * not destroy the menu that runs it. */
  void closeIfAnswered();

  ISoundPlayer* sounds_;
  LeaderboardFilterChoice draft_;
  std::optional<Menu> dialog_;
  bool answered_ = false;
};

}  // namespace rtype::client

#pragma once

#include <SFML/Window/Event.hpp>
#include <functional>
#include <optional>
#include <string>
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "Menu.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The dialog over the options that asks a question before a reset, and
 * takes every event until it is answered.
 *
 * Its buttons hold a pointer to it, so it can be neither copied nor moved.
 */
class ResetDialog {
 public:
  /** @param sounds Plays the click of the dialog; it must outlive the dialog. */
  explicit ResetDialog(ISoundPlayer& sounds);

  ResetDialog(const ResetDialog&) = delete;
  ResetDialog& operator=(const ResetDialog&) = delete;
  ResetDialog(ResetDialog&&) = delete;
  ResetDialog& operator=(ResetDialog&&) = delete;
  ~ResetDialog() = default;

  /** @brief Opens the dialog on @p question; YES runs @p reset and closes it,
   * NO closes it. */
  void ask(const std::string& question, const std::function<void()>& reset);

  [[nodiscard]] bool isOpen() const;

  /** @brief Gives @p event to the dialog, which must be open. */
  void handleEvent(const sf::Event& event, const PlayfieldMapping& pointAt);

  /** @brief Lets @p elapsed pass for the dialog, which must be open. */
  void advance(engine::Duration elapsed);

  /** @brief Paints the dialog when it is open. */
  void paint(IInterfaceCanvas& canvas) const;

 private:
  /** @brief Closes the dialog once one of its buttons has run: an action must
   * not destroy the menu that runs it. */
  void closeIfAnswered();

  ISoundPlayer* sounds_;
  std::optional<Menu> dialog_;
  bool answered_ = false;
};

}  // namespace rtype::client

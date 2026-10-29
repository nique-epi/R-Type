#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <array>
#include <cstddef>
#include <functional>
#include <optional>
#include <vector>
#include "Button.hpp"
#include "HelpWalkthroughConstants.hpp"
#include "HelpWalkthroughParts.hpp"
#include "HelpWord.hpp"
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "LobbyPartAreas.hpp"
#include "MotionPreference.hpp"
#include "PlayerPalette.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"

namespace sf {
class Event;
}  // namespace sf

namespace rtype::client {

/**
 * @brief The help of the waiting room: a HELP chip beside BACK, then, once
 * opened, Professor Bydo explaining the room and a game one part at a time.
 *
 * Closed, it paints its chip only; HELP_KEY, or a click on the chip, opens it
 * on its first step. Open, it takes every event but those that end an input
 * (the release of a key or of a button, the focus lost), which go on to the
 * room so that an input started before it opened ends there too: a key of
 * HELP_NEXT_KEYS or a click shows the next step, the last one closing it; a key
 * of HELP_PREVIOUS_KEYS shows the previous one; MENU_CANCEL_KEY closes it. Each
 * step dims the picture with the layer of dialogs but for the part it explains,
 * framed by four corners, and writes its sentence in a bubble the professor
 * stands beside; a step of HelpScene::Game paints a moment of a game in place
 * of the room. Nothing it shows goes to the server: the room keeps updating
 * under it, and the game, once it starts, replaces both.
 */
class HelpWalkthrough {
 public:
  explicit HelpWalkthrough(const HelpWalkthroughParts& parts);

  /**
   * @returns Whether the event was the help's: any event but one that ends
   * an input while it is open; HELP_KEY or a click on its chip while it is
   * closed. The room handles the event itself only otherwise.
   */
  [[nodiscard]] bool handleEvent(const sf::Event& event,
                                 const PlayfieldMapping& playfieldPointAt);

  void advance(engine::Duration elapsed);

  /** @returns Whether the step shown paints a moment of a game, the room then
   * left unpainted. */
  [[nodiscard]] bool showsGame() const;

  /** @brief Paints the chip, or the moment of a game, then the step shown, if
   * any, over what the room painted. */
  void paint(IInterfaceCanvas& canvas, const LobbyPartAreas& room) const;

 private:
  /** @returns Whether a closed help is asked to open: HELP_KEY pressed, or
   * its chip clicked. */
  [[nodiscard]] bool isAskedToOpen(
      const sf::Event& event, const PlayfieldMapping& playfieldPointAt) const;

  /** @brief Shows the step an event asks an open help for, if it asks for
   * another one. */
  void followStep(const sf::Event& event);

  /** @brief Shows a step, none closing the help, with the click of the
   * menus. */
  void show(std::optional<std::size_t> step);

  /** @brief Paints a step: the dim layer and its corners, the bubble, then the
   * professor beside it. */
  void paintStep(IInterfaceCanvas& canvas, const LobbyPartAreas& room,
                 std::size_t step) const;

  /** @returns The time the animations are shown at: the time since the help
   * was built, none while the system asks for less motion. */
  [[nodiscard]] engine::Duration shownTime() const;

  ISoundPlayer* sounds_{};
  /** @brief The lines of every step, its keys named when the help was
   * built. */
  std::array<std::vector<std::vector<HelpWord>>, HELP_STEPS.size()> stepLines_;
  std::reference_wrapper<const PlayerPalette> palette_;
  MotionPreference motion_{};
  sf::IntRect chipArea_;
  Button chip_;
  engine::Duration elapsed_{};
  std::optional<std::size_t> step_;
};

}  // namespace rtype::client

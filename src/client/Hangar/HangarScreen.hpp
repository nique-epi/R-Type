#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include "FittingRun.hpp"
#include "HangarScreenParts.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "KeyChip.hpp"
#include "MotionPreference.hpp"
#include "PlayerPalette.hpp"
#include "PlayfieldMapping.hpp"
#include "SettingsService.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The hangar, as the design system "R-Type Hangar" draws it: the
 * glass locker of the items on the left, the ship held by four white arms on
 * the right, the reference card of the chosen item above it.
 *
 * The arrows, or the pointer, choose an item; MENU_CONFIRM_KEY, or a click,
 * wears it, or takes it off when the ship wears it. Wearing changes the
 * settings at once, saved on the next update; then the top right arm (the
 * bottom right one for the mustache) lets go of the hull, reaches off screen,
 * brings the item and sets it on the ship over FITTING_DURATION, during which
 * MENU_CONFIRM_KEY waits. Under reduced motion the item shows at once. BACK,
 * or MENU_CANCEL_KEY, pops the screen.
 */
class HangarScreen : public IScreen {
 public:
  /** @param canvas Picture the hangar is painted on, never null. */
  HangarScreen(HangarScreenParts parts,
               std::unique_ptr<IInterfaceCanvas> canvas);
  /** @brief The action of BACK points at the screen, so it never moves. */
  HangarScreen(const HangarScreen&) = delete;
  HangarScreen& operator=(const HangarScreen&) = delete;
  HangarScreen(HangarScreen&&) = delete;
  HangarScreen& operator=(HangarScreen&&) = delete;
  ~HangarScreen() override = default;

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Moves the choice with an arrow, or confirms it once per hold. */
  void keyPressed(const sf::Event::KeyPressed& key);
  /** @brief Wears the chosen item, an arm bringing it, or takes it off; does
   * nothing while an arm fits an item. */
  void choose();
  void repaint();

  IScreenNavigator* navigator_;
  ISoundPlayer* sounds_;
  SettingsService* settings_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  std::reference_wrapper<const PlayerPalette> palette_;
  int shipColor_;
  MotionPreference motion_;
  KeyChip backChip_;
  std::size_t chosen_{};
  std::optional<FittingRun> fitting_;
  engine::Duration elapsed_{};
  bool confirmHeld_{false};
};

}  // namespace rtype::client

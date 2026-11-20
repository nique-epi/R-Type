#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <optional>
#include "AmbienceScope.hpp"
#include "CasinoHallScreenParts.hpp"
#include "HallExit.hpp"
#include "HallPainter.hpp"
#include "ICasinoChannel.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "KeyChip.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The main hall of the casino, pushed over its doors, which it hides:
 * a boxing ring where two Bydo spar, its crowd, the arch to the cashier on
 * the left and the arch to the roulette on the right.
 *
 * ROULETTE, HALL_RIGHT_KEY or a click on the right arch, seats the player in
 * the first room of the master with a seat free (ICasinoChannel::enter())
 * and pushes ROULETTE_SCREEN, the bar waiting until the master listed such a
 * room; the rooms are asked as the hall opens, then every
 * HALL_ROOMS_REFRESH. CASHIER, HALL_LEFT_KEY or a click on the left arch,
 * pushes the cashier. Either fades the hall to the night for HALL_FADE_TIME
 * first, the bars taking nothing meanwhile. MENU_CANCEL_KEY gets the player
 * up from its room (ICasinoChannel::leave()) and walks back to the doors.
 * Each bar acts once per hold of its key, on its release; before a screen
 * is pushed over the hall, its bars drop what they held.
 *
 * With MotionPreference::Reduced, the hall stays still and opens a room at
 * once.
 */
class CasinoHallScreen final : public IScreen {
 public:
  /** @param canvas Picture the hall is painted on, never null. */
  CasinoHallScreen(CasinoHallScreenParts parts,
                   std::unique_ptr<IInterfaceCanvas> canvas);

  /** @brief The actions of the bars point at the screen, so it never
   * moves. */
  CasinoHallScreen(const CasinoHallScreen&) = delete;
  CasinoHallScreen& operator=(const CasinoHallScreen&) = delete;
  CasinoHallScreen(CasinoHallScreen&&) = delete;
  CasinoHallScreen& operator=(CasinoHallScreen&&) = delete;
  ~CasinoHallScreen() override = default;

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Starts walking to @p exit: the fade, or the room at once with
   * MotionPreference::Reduced; the roulette only once a room has a seat. */
  void walkTo(HallExit exit);
  /** @brief Opens the room the player walks to, its bars dropping what they
   * held first. */
  void openChosenRoom();
  /** @brief Gets the player up from its room, then walks back to the
   * doors. */
  void leave();
  /** @returns Whether @p event was a click on an arch, which it then acts
   * on, on the release of a press over the same arch. */
  bool clickOnArch(const sf::Event& event);
  void repaint();

  IScreenNavigator* navigator_;
  /** @brief Holds the music of the room for as long as it shows. */
  AmbienceScope ambience_;
  ICasinoChannel* casino_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  MotionPreference motion_;
  HallPainter painter_;
  /** @brief How long the hall has been open: where its lights are. */
  engine::Duration since_{};
  /** @brief How long ago the rooms were asked last. */
  engine::Duration sinceRoomsAsked_{};
  /** @brief The room the player walks to, and for how long it has. */
  std::optional<HallExit> leavingFor_;
  engine::Duration sinceLeaving_{};
  /** @brief The arch the left button went down on, until it comes up. */
  std::optional<HallExit> pressedArch_;
  KeyChip cashierChip_;
  KeyChip rouletteChip_;
  KeyChip backChip_;
};

}  // namespace rtype::client

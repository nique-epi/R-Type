#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include "DestinationCard.hpp"
#include "DestinationScreenConstants.hpp"
#include "DestinationScreenParts.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "KeyChip.hpp"
#include "PlayfieldMapping.hpp"
#include "ServerChoice.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

struct PlayerSettings;

/**
 * @brief Where PLAY leads, pushed over the title, which it hides: four
 * cards on a turning stand of three slots, SERVER, SHOP, CASINO and HANGAR,
 * the one in the middle large and framed, the two at its sides small and
 * dimmed, the fourth hidden behind them.
 *
 * The arrows turn the stand, a click on a card at a side brings it to the
 * middle; GO, its key MENU_CONFIRM_KEY, or a click on the card in the middle
 * pushes the screen of that card; BACK, MENU_CANCEL_KEY, closes the screen.
 * SERVER opens the server map, or, for a client launched with `-h`, the
 * connection to the server given at launch. CASINO opens the same screen,
 * and marks the choice of the server so the waiting room it leads to opens
 * the casino over itself (ServerChoice::towardsTheCasino); the other cards
 * clear the mark. The balance of kopecks stands
 * at its usual place, top right.
 */
class DestinationScreen : public IScreen {
 public:
  /** @param canvas Picture the screen is painted on, never null. */
  DestinationScreen(DestinationScreenParts parts,
                    std::unique_ptr<IInterfaceCanvas> canvas);
  /** @brief The actions of the chips point at the screen, so it never
   * moves. */
  DestinationScreen(const DestinationScreen&) = delete;
  DestinationScreen& operator=(const DestinationScreen&) = delete;
  DestinationScreen(DestinationScreen&&) = delete;
  DestinationScreen& operator=(DestinationScreen&&) = delete;
  ~DestinationScreen() override = default;

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Turns the stand @p steps cards to the left: the card that many
   * places to the right of the middle comes to the middle. */
  void turnBy(std::size_t steps);
  /** @brief Pushes the screen of the card in the middle. */
  void openChosen();
  /** @brief Acts on a click at @p pixel of the interface: opens the card in
   * the middle, or brings a card at a side to the middle. */
  void pointerPressed(sf::Vector2i pixel);
  void repaint();

  IScreenNavigator* navigator_;
  ISoundPlayer* sounds_;
  const PlayerSettings* settings_;
  ServerChoice* choice_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  std::array<DestinationCard, DESTINATION_COUNT> cards_;
  std::size_t chosen_{};
  /** @brief Nothing on the screen moves: it is painted again only after an
   * event, or when the balance it shows changed under it, in the shop. */
  bool repaintDue_{};
  std::uint32_t paintedKopecks_{};
  KeyChip goChip_;
  KeyChip backChip_;
};

}  // namespace rtype::client

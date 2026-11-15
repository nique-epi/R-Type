#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include "ClothConstants.hpp"
#include "ClothPlace.hpp"
#include "ICasinoChannel.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "ISoundPlayer.hpp"
#include "IWaitingRoom.hpp"
#include "MotionPreference.hpp"
#include "PlayerPalette.hpp"
#include "PlayerSettings.hpp"
#include "PlayfieldMapping.hpp"
#include "RouletteBars.hpp"
#include "RouletteMemory.hpp"
#include "RouletteReading.hpp"
#include "RouletteRound.hpp"
#include "RouletteScreenConstants.hpp"
#include "RouletteScreenParts.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The roulette table of the waiting room, pushed over the hall, which
 * it hides: the wheel, the cloth, the pilots seated and the band.
 *
 * While the bets are open, the arrows move a cursor over every place of the
 * cloth (placeAfter()), and LAY_CHIP_KEY lays there a chip of the value
 * chosen, by its key 1 to 5 or a click on it; the table takes no chip past
 * the chips held, MAXIMUM_ROUND_STAKE or MAXIMUM_ROULETTE_BETS
 * (RouletteSlip). CLEAR takes the bets of the round off, REBET lays again
 * those of the round before, SPIN asks to spin; each change sends every bet
 * with the vote (ICasinoChannel::placeBets()). The chips of the bets are
 * settled by ChipSettlement, never here.
 *
 * The wheel turns once the bets close and stops on the number drawn; the
 * result lights it on the cloth. BACK, MENU_CANCEL_KEY, closes the table,
 * back to the hall; the player stays seated.
 *
 * Every key acts once per hold, the arrows aside, which keep moving the
 * cursor while held. With MotionPreference::Reduced, the wheel stays still
 * until the result.
 */
class RouletteScreen final : public IScreen {
 public:
  /** @param canvas Picture the table is painted on, never null. */
  RouletteScreen(RouletteScreenParts parts,
                 std::unique_ptr<IInterfaceCanvas> canvas);

  /** @brief The actions of the bars point at the screen, so it never
   * moves. */
  RouletteScreen(const RouletteScreen&) = delete;
  RouletteScreen& operator=(const RouletteScreen&) = delete;
  RouletteScreen(RouletteScreen&&) = delete;
  RouletteScreen& operator=(RouletteScreen&&) = delete;
  ~RouletteScreen() override = default;

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Moves the cursor, lays a chip or chooses its value, as @p key
   * says. */
  void keyPressed(sf::Keyboard::Key key);
  /** @brief Chooses the chip under @p pixel of the interface, if any. */
  void pointerPressed(sf::Vector2i pixel);
  /** @brief Lays a chip of the value chosen under the cursor, while the bets
   * are open, then sends the bets. */
  void layChip();
  /** @brief Sends every bet of the round with the vote. */
  void send();
  [[nodiscard]] RouletteReading readTable() const;
  void repaint();

  IScreenNavigator* navigator_;
  ISoundPlayer* sounds_;
  ICasinoChannel* casino_;
  RouletteMemory* memory_;
  const PlayerSettings* settings_;
  const IServerConnection* connection_;
  const IWaitingRoom* waitingRoom_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  PlayerPalette palette_;
  MotionPreference motion_;
  /** @brief The state of the table seen last, and how long ago it came. */
  std::optional<network::RouletteRound> seenRound_;
  engine::Duration sinceRound_{};
  ClothPlace cursor_{FIRST_CLOTH_PLACE};
  std::size_t chosenChip_{FIRST_CHOSEN_CHIP};
  /** @brief The key pressed last, which acts no more until released. */
  std::optional<sf::Keyboard::Key> heldKey_;
  RouletteBars bars_;
};

}  // namespace rtype::client

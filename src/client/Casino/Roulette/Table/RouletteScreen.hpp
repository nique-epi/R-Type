#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include "AmbienceScope.hpp"
#include "BetsReading.hpp"
#include "CasinoRoomState.hpp"
#include "ClothConstants.hpp"
#include "ClothPlace.hpp"
#include "IAccountSession.hpp"
#include "ICasinoChannel.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "IWallet.hpp"
#include "MotionPreference.hpp"
#include "PlayerPalette.hpp"
#include "PlayfieldMapping.hpp"
#include "RouletteBars.hpp"
#include "RouletteMemory.hpp"
#include "RouletteReading.hpp"
#include "RouletteScreenConstants.hpp"
#include "RouletteScreenParts.hpp"
#include "RoundSounds.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The roulette table of a room of the casino of the master, pushed over
 * the hall, which it hides: the wheel, the cloth, the players seated and the
 * band. The master times the rounds and spins the wheel once the bets close;
 * the table counts down to it, then to the result.
 *
 * While the bets are open, the arrows move a cursor over every place of the
 * cloth (placeAfter()), and LAY_CHIP_KEY lays there a chip of the value
 * chosen, by its key 1 to 5 or a click on it; the table takes no chip past
 * the chips at hand, MAXIMUM_ROUND_STAKE or MAXIMUM_ROULETTE_BETS
 * (RouletteSlip). CLEAR takes the bets of the round off, REBET lays again
 * those of the round before; each change sends every bet
 * (ICasinoChannel::placeBets()), and bets the master refuses leave the cloth
 * as it holds it, the status line saying why. The chips are staked and paid
 * by the master, never here.
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
  /** @brief Sends every bet of the round. */
  void send();
  /** @returns What the table shows of @p room and @p bets, the newest
   * state of the room and answer to the bets, read once per frame. */
  [[nodiscard]] RouletteReading readTable(
      const std::optional<network::CasinoRoomState>& room,
      const std::optional<BetsReading>& bets) const;
  /** @brief Paints the table as @p reading shows it. */
  void repaint(const RouletteReading& reading);

  IScreenNavigator* navigator_;
  /** @brief Holds the music of the room for as long as it shows. */
  AmbienceScope ambience_;
  ISoundPlayer* sounds_;
  RoundSounds roundSounds_;
  ICasinoChannel* casino_;
  RouletteMemory* memory_;
  const IWallet* wallet_;
  const IAccountSession* account_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  PlayerPalette palette_;
  MotionPreference motion_;
  ClothPlace cursor_{FIRST_CLOTH_PLACE};
  std::size_t chosenChip_{FIRST_CHOSEN_CHIP};
  /** @brief The key pressed last, which acts no more until released. */
  std::optional<sf::Keyboard::Key> heldKey_;
  RouletteBars bars_;
};

}  // namespace rtype::client

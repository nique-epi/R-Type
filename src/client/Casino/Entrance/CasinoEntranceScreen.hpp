#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include "CasinoEntranceScreenParts.hpp"
#include "EntranceStage.hpp"
#include "ICasinoChannel.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "IWallet.hpp"
#include "KeyChip.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenId.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The casino, the Bydo Palace, pushed over the server map, which it
 * hides.
 *
 * Before its closed doors, Balthazar, the Bydo butler, welcomes the player:
 * ENTER, its key MENU_CONFIRM_KEY, opens the doors while he greets and bows;
 * any key or a click skips it. BACK, MENU_CANCEL_KEY, closes the screen, back
 * to the map.
 *
 * Inside, the hall shows its menu in a gold panel and the balances of kopecks
 * and chips top right: ROULETTE, MENU_CONFIRM_KEY, seats the player in the
 * first room of the master with a seat free (ICasinoChannel::enter()) and
 * pushes ROULETTE_SCREEN, the bar waiting until the master listed such a
 * room; the rooms are asked as the casino opens, then every
 * HALL_ROOMS_REFRESH. BUY CHIPS, BUY_CHIPS_KEY, and CASH OUT, CASH_OUT_KEY,
 * push the cashier; LEAVE, MENU_CANCEL_KEY, gets the player up from its room
 * (ICasinoChannel::leave()) and closes the screen. Each bar acts once per hold
 * of its key, on its release; before a screen is pushed over the hall, its bars
 * drop what they held, since the events stop reaching them.
 *
 * With MotionPreference::Reduced, ENTER opens the hall at once and the butler
 * stands still.
 */
class CasinoEntranceScreen final : public IScreen {
 public:
  /** @param canvas Picture the casino is painted on, never null. */
  CasinoEntranceScreen(CasinoEntranceScreenParts parts,
                       std::unique_ptr<IInterfaceCanvas> canvas);

  /** @brief The actions of the bars point at the screen, so it never
   * moves. */
  CasinoEntranceScreen(const CasinoEntranceScreen&) = delete;
  CasinoEntranceScreen& operator=(const CasinoEntranceScreen&) = delete;
  CasinoEntranceScreen(CasinoEntranceScreen&&) = delete;
  CasinoEntranceScreen& operator=(CasinoEntranceScreen&&) = delete;
  ~CasinoEntranceScreen() override = default;

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Opens the doors, or the hall at once with
   * MotionPreference::Reduced. */
  void enter();
  /** @brief Gets the player up from its room, then closes the screen. */
  void leave();
  /** @brief Seats the player in the first room with a seat free, then pushes
   * the table; nothing before such a room is known. */
  void playRoulette();
  /** @brief Drops what the bars of the hall held, then pushes @p screen. */
  void openOver(ScreenId screen);
  /** @brief Shows @p stage from its start. */
  void reach(EntranceStage stage);
  void repaint();

  IScreenNavigator* navigator_;
  ICasinoChannel* casino_;
  const IWallet* wallet_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  MotionPreference motion_;
  EntranceStage stage_{EntranceStage::Doors};
  /** @brief How long the casino has shown its stage: where the doors and the
   * butler are. */
  engine::Duration sinceStage_{};
  /** @brief How long ago the rooms were asked last. */
  engine::Duration sinceRoomsAsked_{};
  KeyChip enterChip_;
  KeyChip doorsBackChip_;
  KeyChip rouletteChip_;
  KeyChip buyChipsChip_;
  KeyChip cashOutChip_;
  KeyChip leaveChip_;
};

}  // namespace rtype::client

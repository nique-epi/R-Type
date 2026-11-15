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
#include "KeyChip.hpp"
#include "MotionPreference.hpp"
#include "PlayerSettings.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenId.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The casino, the Bydo Palace, pushed over the waiting room, which it
 * hides.
 *
 * Before its closed doors, Balthazar, the Bydo butler, welcomes the player:
 * ENTER, its key MENU_CONFIRM_KEY, seats the player at the table of the room
 * (ICasinoChannel::chooseTable() with TablePlace::AtTable), then the doors
 * open while he greets and bows; any key or a click skips it. BACK,
 * MENU_CANCEL_KEY, closes the screen, back to the waiting room, the player
 * never seated.
 *
 * Inside, the hall shows its menu in a gold panel and the balances of kopecks
 * and chips top right: ROULETTE, MENU_CONFIRM_KEY, pushes ROULETTE_SCREEN;
 * BUY CHIPS, BUY_CHIPS_KEY, and CASH OUT, CASH_OUT_KEY, push the cashier;
 * LEAVE, MENU_CANCEL_KEY, takes the player back to the room
 * (TablePlace::InRoom) and closes the screen. Each bar acts once per hold of
 * its key, on its release; before a screen is pushed over the hall, its bars
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
  /** @brief Seats the player at the table, then opens the doors, or the hall
   * at once with MotionPreference::Reduced. */
  void enter();
  /** @brief Takes the player back to the room, then closes the screen. */
  void leave();
  /** @brief Drops what the bars of the hall held, then pushes @p screen. */
  void openOver(ScreenId screen);
  /** @brief Shows @p stage from its start. */
  void reach(EntranceStage stage);
  void repaint();

  IScreenNavigator* navigator_;
  ICasinoChannel* casino_;
  const PlayerSettings* settings_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  MotionPreference motion_;
  EntranceStage stage_{EntranceStage::Doors};
  /** @brief How long the casino has shown its stage: where the doors and the
   * butler are. */
  engine::Duration sinceStage_{};
  KeyChip enterChip_;
  KeyChip doorsBackChip_;
  KeyChip rouletteChip_;
  KeyChip buyChipsChip_;
  KeyChip cashOutChip_;
  KeyChip leaveChip_;
};

}  // namespace rtype::client

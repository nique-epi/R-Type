#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include "AmbienceScope.hpp"
#include "CasinoEntranceScreenParts.hpp"
#include "EntranceLightPainter.hpp"
#include "EntranceStage.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "KeyChip.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "SpokenLine.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The front of the casino, the Bydo Palace, pushed over the server
 * map, which it hides: its 777 pylon and its marquee alight, two
 * searchlights in the sky, and Balthazar, the Bydo butler, in front, who
 * welcomes the player in the voice of the Bydo of the shop.
 *
 * ENTER, its key MENU_CONFIRM_KEY, opens the doors while he greets and bows,
 * then pushes CASINO_HALL_SCREEN over the doors, which close behind the
 * player: the hall walks back to them. Any key or a click during the opening
 * skips it. BACK, MENU_CANCEL_KEY, closes the screen, back to the map. Each
 * bar acts once per hold of its key, on its release; before the hall is
 * pushed, the bars drop what they held, since the events stop reaching them.
 *
 * With MotionPreference::Reduced, the lights and the butler stand still and
 * ENTER opens the hall at once.
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
  /** @brief Pushes the hall, the bars dropping what they held first, and
   * closes the doors behind the player. */
  void openTheHall();
  /** @brief Shows @p stage from its start. */
  void reach(EntranceStage stage);
  void repaint();

  IScreenNavigator* navigator_;
  /** @brief Holds the music of the room for as long as it shows. */
  AmbienceScope ambience_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  MotionPreference motion_;
  EntranceLightPainter lights_;
  /** @brief What the butler says, letter by letter in his voice, staying in
   * his panel. */
  SpokenLine butlerLine_;
  EntranceStage stage_{EntranceStage::Doors};
  /** @brief How long the casino has shown its stage: where the doors and the
   * butler are. */
  engine::Duration sinceStage_{};
  /** @brief How long the front has been open: where its lights are. */
  engine::Duration since_{};
  KeyChip enterChip_;
  KeyChip doorsBackChip_;
};

}  // namespace rtype::client

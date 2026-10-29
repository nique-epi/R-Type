#pragma once

#include <memory>
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ISizableWindow.hpp"
#include "OptionsPlace.hpp"
#include "OptionsScreenParts.hpp"
#include "OptionsTabs.hpp"
#include "PlayfieldMapping.hpp"
#include "ResetDialog.hpp"
#include "SavedStamp.hpp"
#include "SettingsService.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The options, in four tabs, SOUND, CONTROLS, DISPLAY and ACCESS, each
 * a panel of rows over BACK.
 *
 * Every row reads and writes the player's settings: a change takes effect at
 * once and is saved at the next update, once a frame at most, so a slider
 * dragged is saved at each step it crosses. SAVED is written at the end
 * of the rule for SAVED_STAMP_DURATION after a save, NOT SAVED until the next
 * save after a failure. BACK, or MENU_CANCEL_KEY, saves what is not saved yet,
 * then closes the options.
 *
 * DEFAULT SOUND and DEFAULT KEYS ask first, in a dialog over the options that
 * takes every event until it is answered. The description of a row says what
 * it needs right now: the keys of an open list, a key awaited, a key two
 * actions share, the sound muted, a size too large for the screen, what
 * borderless does. While the window is borderless, WINDOW SIZE is disabled,
 * shows the size of the screen, and the chart of the sizes is not drawn.
 *
 * Over the title, the options hide it. Over a game, which goes on, they dim it
 * and say so in the top-right corner.
 */
class OptionsScreen : public IScreen {
 public:
  /** @param canvas Picture the options are painted on, never null. */
  OptionsScreen(OptionsScreenParts parts,
                std::unique_ptr<IInterfaceCanvas> canvas);

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Saves the settings when a change is not saved yet, and stamps the
   * result; does nothing otherwise. */
  void save();

  void close();

  /** @brief Writes the descriptions that follow the state of the rows, on
   * every tab, and the stamp, on the open tab. */
  void refresh();

  void repaint();

  IScreenNavigator* navigator_;
  OptionsPlace place_;
  SettingsService* service_;
  ISizableWindow* window_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  ResetDialog dialog_;
  OptionsTabs tabs_;
  SavedStamp stamp_;
};

}  // namespace rtype::client

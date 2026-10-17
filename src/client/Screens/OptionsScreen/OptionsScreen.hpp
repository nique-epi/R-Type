#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "BoundAction.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ISettingsStorage.hpp"
#include "ISizableWindow.hpp"
#include "KeyNames.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "PlayerSettings.hpp"
#include "PlayfieldMapping.hpp"
#include "RowControl.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

class Dropdown;
class KeySlot;

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
 * actions share, the sound muted, a size too large for the screen; the
 * settings without an effect yet say COMING SOON.
 *
 * Over the title, the options hide it. Over a game, which goes on, they dim it
 * and say so in the top-right corner.
 */
class OptionsScreen : public IScreen {
 public:
  /** @brief What the options are opened over. */
  enum class Place : std::uint8_t {
    OverTitle,
    OverGame,
  };

  /**
   * @param navigator Screens the options live among.
   * @param settings What the rows read and change.
   * @param storage Where a change is saved.
   * @param window Window whose size DISPLAY picks.
   * @param keyName Gives the name drawn on the keycap of a key.
   * @param canvas Picture the options are painted on, never null.
   * @param playfieldPointAt Gives the point of the playfield under a pixel of
   * the window.
   *
   * The navigator, the settings, the storage and the window must outlive the
   * screen.
   */
  OptionsScreen(IScreenNavigator& navigator, Place place,
                PlayerSettings& settings, ISettingsStorage& storage,
                ISizableWindow& window, KeyNaming keyName,
                std::unique_ptr<IInterfaceCanvas> canvas,
                PlayfieldMapping playfieldPointAt);

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  [[nodiscard]] std::vector<Menu> buildTabs();
  [[nodiscard]] MenuBuilder tabbedPanel(std::size_t tab);
  [[nodiscard]] Menu withBack(MenuBuilder& builder);
  [[nodiscard]] Menu soundTab();
  [[nodiscard]] Menu controlsTab();
  [[nodiscard]] Menu displayTab();
  [[nodiscard]] Menu accessTab();

  [[nodiscard]] std::unique_ptr<RowControl> volumeSlider(
      int PlayerSettings::* volume);
  [[nodiscard]] std::unique_ptr<RowControl> settingSwitch(
      bool PlayerSettings::* setting);
  [[nodiscard]] std::unique_ptr<KeySlot> keySlot(BoundAction action);
  [[nodiscard]] std::unique_ptr<RowControl> resetLink(
      std::string question, std::function<void()> reset);

  void openTab(std::size_t tab);
  void chooseWindowSize(std::size_t index);
  void resetSound();
  void resetKeys();

  /** @brief Opens the dialog that asks a question before a reset. */
  void askToReset(const std::string& question,
                  const std::function<void()>& reset);

  /** @brief Closes the dialog once one of its buttons has run: an action must
   * not destroy the menu that runs it. */
  void closeDialogIfAnswered();

  void changed();

  /** @brief Saves the settings when a change is not saved yet, and stamps the
   * result; does nothing otherwise. */
  void save();

  void close();
  void ageStamp(engine::Duration elapsed);

  /** @brief Writes the descriptions that follow the state of the rows, on
   * every tab, and the stamp, on the open tab. */
  void refresh();

  [[nodiscard]] std::string keyDescription(std::size_t row) const;
  [[nodiscard]] std::string windowSizeDescription() const;

  [[nodiscard]] Menu& openMenu();
  [[nodiscard]] const Menu& openMenu() const;
  void repaint();

  IScreenNavigator* navigator_;
  Place place_;
  PlayerSettings* settings_;
  ISettingsStorage* storage_;
  ISizableWindow* window_;
  KeyNaming keyName_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  /** @brief The list of sizes and the key slots, owned by their rows; set
   * while tabs_ is built, so declared, and initialized, before it. */
  Dropdown* windowSizes_ = nullptr;
  std::array<KeySlot*, BOUND_ACTION_COUNT> keySlots_{};
  std::vector<Menu> tabs_;
  std::size_t openTab_ = 0;
  std::optional<Menu> dialog_;
  bool dialogAnswered_ = false;
  bool unsaved_ = false;
  std::string stamp_;
  /** @brief How long SAVED has been written; nothing while no stamp fades. */
  std::optional<engine::Duration> stampAge_;
};

}  // namespace rtype::client

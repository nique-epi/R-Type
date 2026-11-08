#pragma once

#include <memory>
#include <string_view>
#include "Endpoint.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "MasterScreenParts.hpp"
#include "Menu.hpp"
#include "PlayfieldMapping.hpp"
#include "SettingsService.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The form MASTER opens over the server map, which it hides: the
 * address and the port of the master the map reads, in the fields of DIRECT
 * CONNECT (see addEndpointFields()), then SAVE and BACK, and a status line
 * naming a field to fix.
 *
 * Its fields start with the master read when it opens; for a master the
 * master-url setting names by host, an empty address and DEFAULT_MASTER_PORT.
 * MENU_CONFIRM_KEY on a field, or SAVE, checks the fields: an address that is
 * not IPv4 or a port outside LOWEST_PORT to HIGHEST_PORT is named on the status
 * line, and nothing is kept. Otherwise SAVE gives MASTER_SETTING that master,
 * saves the settings and closes the form; the map then reads the servers of
 * that master. BACK, or MENU_CANCEL_KEY, closes the form and keeps the master
 * as it was.
 */
class MasterScreen : public IScreen {
 public:
  /** @param canvas Picture the form is painted on, never null. */
  MasterScreen(MasterScreenParts parts,
               std::unique_ptr<IInterfaceCanvas> canvas);

  /** @brief The actions of the menu point at the screen, so it never moves. */
  MasterScreen(const MasterScreen&) = delete;
  MasterScreen& operator=(const MasterScreen&) = delete;
  MasterScreen(MasterScreen&&) = delete;
  MasterScreen& operator=(MasterScreen&&) = delete;
  ~MasterScreen() override = default;

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Builds the form, its fields filled with @p shown. */
  [[nodiscard]] Menu buildMenu(ISoundPlayer& sounds,
                               const network::Endpoint& shown);

  /** @brief Keeps the master the fields name, then closes the form; or names
   * the field to fix. */
  void save();

  /** @brief Paints the form and its status line. */
  void repaint();

  IScreenNavigator* navigator_{};
  SettingsService* settings_{};
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  std::string_view problem_;
  Menu menu_;
};

}  // namespace rtype::client

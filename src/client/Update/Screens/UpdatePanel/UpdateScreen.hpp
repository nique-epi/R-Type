#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <string>
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "IUpdateRun.hpp"
#include "TimeConstants.hpp"
#include "UpdatePhase.hpp"
#include "UpdateProgress.hpp"
#include "UpdateScreenParts.hpp"

namespace rtype::client {

/**
 * @brief The panel that updates the client, over the dimmed title: its
 * heading, the versions, a bar of the bytes received, and a status line.
 *
 * It keeps every event, so the player can do nothing until it ends: once the
 * new version is ready it shows RESTARTING for RESTART_MESSAGE_DURATION, then
 * starts it and closes the client; once the update failed it shows UPDATE
 * FAILED for FAILED_MESSAGE_DURATION, then closes and lets the player play.
 */
class UpdateScreen : public IScreen {
 public:
  /** @param canvas Picture the panel is painted on, never null. */
  UpdateScreen(UpdateScreenParts parts,
               std::unique_ptr<IInterfaceCanvas> canvas);

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Restarts or closes once the message of the phase showed long
   * enough; once only, since the change it asks for applies after the frame. */
  void endIfDue(UpdatePhase phase);
  /** @brief Paints the panel as @p progress stands. */
  void repaint(const UpdateProgress& progress);

  std::unique_ptr<IInterfaceCanvas> canvas_;
  IScreenNavigator* navigator_;
  IUpdateRun* run_;
  std::string versionLine_;
  UpdatePhase shownPhase_{UpdatePhase::Downloading};
  engine::Duration inPhase_{};
  bool ended_{};
};

}  // namespace rtype::client

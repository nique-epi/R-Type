#pragma once

#include <memory>
#include <string>
#include <vector>
#include "AssetLibrary.hpp"
#include "EventListeners.hpp"
#include "GameWindow.hpp"
#include "IModule.hpp"
#include "IPlayerRoundTrips.hpp"
#include "ISessionArrivals.hpp"
#include "InstallContext.hpp"
#include "InterfaceCanvas.hpp"
#include "Lagometer.hpp"
#include "LagometerReading.hpp"
#include "LagometerToggle.hpp"
#include "PlayerSettings.hpp"
#include "ServerSession.hpp"
#include "SubscriptionHandle.hpp"
#include "TimeConstants.hpp"
#include "TrafficCounters.hpp"

namespace rtype::client {

/**
 * @brief The module that shows the lagometer over every screen: the ping, its
 * jitter, the datagrams of the server missed, the frames per second and the
 * traffic, then the curve of the round trips, as measured, with each lost
 * datagram marked in red (see Lagometer, paintLagometerPanel() and
 * lagometerCurve()).
 *
 * It reads what it shows each frame in UPDATE_STAGE, shown or not, so the
 * curve already holds the recent round trips when LAGOMETER_KEY shows it.
 * It draws in RENDER_STAGE, after the screens, which it is listed after: the
 * panel on an interface picture of its own, then the curve at the resolution
 * of the window. Its listener of the window events, added in start(), hears
 * LAGOMETER_KEY before the screen on top. No module requires it, so the
 * client runs the same without it.
 */
class ClientDebugModule final : public engine::IModule {
 public:
  /** @returns DEBUG_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns The assets, the settings, the window and the connection, which
   * provide what it reads and draws on. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException when a module it depends on did not
   * provide what it reads. */
  void install(engine::InstallContext& context) override;
  /** @brief Builds the interface picture of the panel and listens to the
   * window events.
   * @throws What InterfaceCanvas throws when the fonts are not loaded. */
  void start() override;
  /** @brief Stops listening to the window events and drops the picture. */
  void stop() override;

 private:
  /** @returns What the lagometer reads this frame. */
  [[nodiscard]] LagometerReading readingOf(engine::Duration elapsed) const;

  /** @brief Draws the panel, then the curve, while the lagometer is shown. */
  void draw();

  const AssetLibrary* assets_{nullptr};
  const PlayerSettings* settings_{nullptr};
  GameWindow* window_{nullptr};
  EventListeners* listeners_{nullptr};
  const ServerSession* session_{nullptr};
  const IPlayerRoundTrips* roundTrips_{nullptr};
  const ISessionArrivals* arrivals_{nullptr};
  const network::TrafficCounters* traffic_{nullptr};
  std::unique_ptr<InterfaceCanvas> canvas_;
  engine::SubscriptionHandle listening_{};
  LagometerToggle toggle_;
  Lagometer lagometer_;
};

}  // namespace rtype::client

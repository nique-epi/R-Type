#include "ClientDebugModule.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <string>
#include <vector>
#include "AssetLibrary.hpp"
#include "ClientConstants.hpp"
#include "EventListeners.hpp"
#include "GameWindow.hpp"
#include "IPlayerRoundTrips.hpp"
#include "ISessionArrivals.hpp"
#include "InstallContext.hpp"
#include "InterfaceCanvas.hpp"
#include "LagometerLook.hpp"
#include "LagometerPainting.hpp"
#include "LagometerReading.hpp"
#include "ModuleConstants.hpp"
#include "PlayerSettings.hpp"
#include "ServerSession.hpp"
#include "TimeConstants.hpp"
#include "TrafficCounters.hpp"
#include "TrafficReport.hpp"

namespace rtype::client {

std::string ClientDebugModule::name() const {
  return std::string{DEBUG_MODULE_NAME};
}

std::vector<std::string> ClientDebugModule::dependencies() const {
  return {std::string{ASSETS_MODULE_NAME}, std::string{SETTINGS_MODULE_NAME},
          std::string{WINDOW_MODULE_NAME}, std::string{CONNECTION_MODULE_NAME}};
}

void ClientDebugModule::install(engine::InstallContext& context) {
  assets_ = &context.require<AssetLibrary>();
  settings_ = &context.require<PlayerSettings>();
  window_ = &context.require<GameWindow>();
  listeners_ = &context.require<EventListeners>();
  session_ = &context.require<ServerSession>();
  roundTrips_ = &context.require<IPlayerRoundTrips>();
  arrivals_ = &context.require<ISessionArrivals>();
  traffic_ = &context.require<network::TrafficCounters>();
  context.addWork(UPDATE_STAGE, [this](engine::Duration elapsed) {
    lagometer_.update(readingOf(elapsed));
  });
  context.addWork(
      RENDER_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { draw(); });
}

void ClientDebugModule::start() {
  canvas_ = std::make_unique<InterfaceCanvas>(*assets_);
  listening_ = listeners_->add(
      [this](const sf::Event& event) { return toggle_.handle(event); });
}

void ClientDebugModule::stop() {
  listeners_->remove(listening_);
  canvas_.reset();
}

LagometerReading ClientDebugModule::readingOf(engine::Duration elapsed) const {
  const network::TrafficReport traffic = traffic_->report();
  return LagometerReading{
      .frame = elapsed,
      .figures = roundTrips_->roundTripOf(session_->status().playerNumber),
      .newestRoundTrip = roundTrips_->newestOwnRoundTrip(),
      .arrivals = arrivals_->arrivals(),
      .bytesReceived = traffic.others.received.datagrams.bytes,
      .bytesSent = traffic.others.sent.datagrams.bytes};
}

void ClientDebugModule::draw() {
  if (!toggle_.shown() || canvas_ == nullptr) {
    return;
  }
  const LagometerLook look{.scale = settings_->lagometerScale,
                           .fillPercent = settings_->lagometerBackground};
  canvas_->clear();
  paintLagometerPanel(*canvas_, lagometer_, look);
  canvas_->display();
  sf::RenderTarget& target = window_->renderTarget();
  canvas_->show(target);
  target.draw(lagometerCurve(lagometer_, look.scale));
}

}  // namespace rtype::client

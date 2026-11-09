#include "ClientDebugModule.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "AssetLibrary.hpp"
#include "ClientConstants.hpp"
#include "EventListeners.hpp"
#include "GameWindow.hpp"
#include "IOwnRoundTrip.hpp"
#include "ISessionArrivals.hpp"
#include "InstallContext.hpp"
#include "InterfaceCanvas.hpp"
#include "InterpolationTally.hpp"
#include "LagometerConstants.hpp"
#include "LagometerJournalFigures.hpp"
#include "LagometerLook.hpp"
#include "LagometerPainting.hpp"
#include "LagometerReading.hpp"
#include "LagometerText.hpp"
#include "ModuleConstants.hpp"
#include "PlayerSettings.hpp"
#include "TimeConstants.hpp"
#include "TrafficCounters.hpp"
#include "TrafficTotals.hpp"

namespace rtype::client {

ClientDebugModule::ClientDebugModule()
    : logger_(std::string{LAGOMETER_LOGGER_NAME}) {}

std::string ClientDebugModule::name() const {
  return std::string{DEBUG_MODULE_NAME};
}

std::vector<std::string> ClientDebugModule::dependencies() const {
  return {std::string{ASSETS_MODULE_NAME}, std::string{SETTINGS_MODULE_NAME},
          std::string{WINDOW_MODULE_NAME}, std::string{CONNECTION_MODULE_NAME},
          std::string{GAME_MODULE_NAME}};
}

void ClientDebugModule::install(engine::InstallContext& context) {
  assets_ = &context.require<AssetLibrary>();
  settings_ = &context.require<PlayerSettings>();
  window_ = &context.require<GameWindow>();
  listeners_ = &context.require<EventListeners>();
  roundTrip_ = &context.require<IOwnRoundTrip>();
  arrivals_ = &context.require<ISessionArrivals>();
  traffic_ = &context.require<network::TrafficCounters>();
  interpolation_ = &context.require<InterpolationTally>();
  context.addWork(UPDATE_STAGE, [this](engine::Duration elapsed) {
    take(readingOf(elapsed));
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
  LagometerReading reading{.frame = elapsed,
                           .figures = roundTrip_->ownFigures(),
                           .newestRoundTrip = roundTrip_->newestOwnRoundTrip(),
                           .arrivals = arrivals_->arrivals(),
                           .bytesReceived = 0,
                           .bytesSent = 0,
                           .pastTheNewest = interpolation_->pastTheNewest(),
                           .delayTicks = interpolation_->delayTicks()};
  if (lagometer_.readsTrafficAfter(elapsed)) {
    const network::TrafficTotals traffic =
        network::trafficTotalsOf(traffic_->report());
    reading.bytesReceived = traffic.received.bytes;
    reading.bytesSent = traffic.sent.bytes;
  }
  return reading;
}

void ClientDebugModule::take(const LagometerReading& reading) {
  lagometer_.update(reading);
  const std::optional<LagometerJournalFigures> period =
      journal_.update(reading);
  if (period.has_value()) {
    logger_.info(lagometerJournalLine(*period));
  }
}

void ClientDebugModule::draw() {
  if (!toggle_.shown() || canvas_ == nullptr) {
    return;
  }
  const LagometerLook look{.scale = settings_->lagometerScale,
                           .fillPercent = settings_->lagometerBackground};
  repaintWhenChanged(look);
  sf::RenderTarget& target = window_->renderTarget();
  canvas_->show(target);
  target.draw(curve_);
}

void ClientDebugModule::repaintWhenChanged(LagometerLook look) {
  if (paintedRevision_ == lagometer_.revision() && paintedLook_ == look) {
    return;
  }
  canvas_->clear();
  paintLagometerPanel(*canvas_, lagometer_, look);
  canvas_->display();
  curve_ = lagometerCurve(lagometer_, look.scale);
  paintedRevision_ = lagometer_.revision();
  paintedLook_ = look;
}

}  // namespace rtype::client

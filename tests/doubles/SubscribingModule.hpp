#pragma once

#include <optional>
#include <string>
#include <vector>
#include "DeliveredEvents.hpp"
#include "EventBus.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "SubscriptionHandle.hpp"

/**
 * @brief Module double written as a module that listens to the bus must be:
 * install() requires the EventBus and keeps it, start() subscribes to
 * TestEvent and keeps the handle, stop() unsubscribes with it. Each TestEvent
 * delivered is counted in a DeliveredEvents the test owns.
 */
class SubscribingModule : public rtype::engine::IModule {
 public:
  /** @param delivered Where the events are counted; it must outlive the
   * module. */
  SubscribingModule(std::string name, DeliveredEvents& delivered);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException if no module installed before provides
   * an EventBus. */
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  std::string name_;
  DeliveredEvents* delivered_;
  rtype::engine::EventBus* bus_{nullptr};
  std::optional<rtype::engine::SubscriptionHandle> subscription_;
};

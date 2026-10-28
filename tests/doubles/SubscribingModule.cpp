#include "SubscribingModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "DeliveredEvents.hpp"
#include "EventBus.hpp"
#include "InstallContext.hpp"
#include "TestEvent.hpp"

SubscribingModule::SubscribingModule(std::string name,
                                     DeliveredEvents& delivered)
    : name_(std::move(name)), delivered_(&delivered) {}

std::string SubscribingModule::name() const { return name_; }

std::vector<std::string> SubscribingModule::dependencies() const { return {}; }

void SubscribingModule::install(rtype::engine::InstallContext& context) {
  bus_ = &context.require<rtype::engine::EventBus>();
}

void SubscribingModule::start() {
  subscription_ = bus_->subscribe<TestEvent>(
      [this]([[maybe_unused]] const TestEvent& event) { ++delivered_->count; });
}

void SubscribingModule::stop() {
  if (subscription_.has_value()) {
    bus_->unsubscribe(*subscription_);
    subscription_.reset();
  }
}

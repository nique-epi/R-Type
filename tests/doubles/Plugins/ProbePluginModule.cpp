#include "ProbePluginModule.hpp"
#include <string>
#include <vector>
#include "DeliveredEvents.hpp"
#include "EventBus.hpp"
#include "InstallContext.hpp"
#include "PluginLifetime.hpp"
#include "TestEvent.hpp"
#include "TestPluginConstants.hpp"

ProbePluginModule::~ProbePluginModule() {
  if (lifetime_ != nullptr) {
    lifetime_->moduleDestroyed = true;
  }
}

std::string ProbePluginModule::name() const {
  return std::string{PROBE_MODULE_NAME};
}

std::vector<std::string> ProbePluginModule::dependencies() const {
  return {std::string{PLUGIN_HOST_MODULE_NAME}};
}

void ProbePluginModule::install(rtype::engine::InstallContext& context) {
  bus_ = &context.require<rtype::engine::EventBus>();
  delivered_ = &context.require<DeliveredEvents>();
  lifetime_ = context.find<PluginLifetime>();
}

void ProbePluginModule::start() {
  subscription_ = bus_->subscribe<TestEvent>(
      [this]([[maybe_unused]] const TestEvent& event) { ++delivered_->count; });
}

void ProbePluginModule::stop() {
  if (subscription_.has_value()) {
    bus_->unsubscribe(*subscription_);
    subscription_.reset();
  }
}

#pragma once

#include "DeliveredEvents.hpp"
#include "EventBus.hpp"
#include "PluginLifetime.hpp"

/** @brief What a PluginHostModule provides to the plugins of a test: the bus
 * they subscribe to, the count of events they received and the record of their
 * module being destroyed. The test owns it and publishes on the bus. */
struct PluginHostState {
  rtype::engine::EventBus bus;
  DeliveredEvents delivered;
  PluginLifetime lifetime;
};

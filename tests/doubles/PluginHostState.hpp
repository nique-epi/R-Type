#pragma once

#include "DeliveredEvents.hpp"
#include "EventBus.hpp"
#include "PluginLifetime.hpp"
#include "RecordingJournal.hpp"

/** @brief What a PluginHostModule provides to the plugins of a test: the bus
 * they subscribe to, the count of events they received and the record of their
 * module being destroyed, and the journal they write their lines to. The test
 * owns it and publishes on the bus. */
struct PluginHostState {
  rtype::engine::EventBus bus;
  DeliveredEvents delivered;
  PluginLifetime lifetime;
  RecordingJournal journal;
};

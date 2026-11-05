#pragma once

#include <optional>
#include <string>
#include <vector>
#include "DeliveredEvents.hpp"
#include "EventBus.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "PluginLifetime.hpp"
#include "SubscriptionHandle.hpp"

/**
 * @brief The module of the probe plugin, built inside its shared library: it
 * depends on the host, requires the EventBus and the DeliveredEvents the host
 * provides, counts each TestEvent it receives, and records its own destruction
 * in the PluginLifetime when the host provides one.
 */
class ProbePluginModule : public rtype::engine::IModule {
 public:
  ProbePluginModule() = default;
  ProbePluginModule(const ProbePluginModule&) = delete;
  ProbePluginModule& operator=(const ProbePluginModule&) = delete;
  ProbePluginModule(ProbePluginModule&&) = delete;
  ProbePluginModule& operator=(ProbePluginModule&&) = delete;
  ~ProbePluginModule() override;

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException if the host provides no EventBus or no
   * DeliveredEvents. */
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  rtype::engine::EventBus* bus_{nullptr};
  DeliveredEvents* delivered_{nullptr};
  PluginLifetime* lifetime_{nullptr};
  std::optional<rtype::engine::SubscriptionHandle> subscription_;
};

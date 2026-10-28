#pragma once

#include <atomic>
#include <cstddef>
#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"

/**
 * @brief Module double that counts the passes of the server loop: during its
 * install() it adds to SIMULATION_STAGE a work that adds one to a counter the
 * test reads from another thread. It depends on NETWORK_MODULE_NAME and opens
 * nothing.
 */
class TickCountingModule : public rtype::engine::IModule {
 public:
  /** @param passes Where the passes are counted; it must outlive the module.
   */
  explicit TickCountingModule(std::atomic<std::size_t>& passes);

  /** @returns TICK_COUNTING_MODULE. */
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  std::atomic<std::size_t>* passes_;
};

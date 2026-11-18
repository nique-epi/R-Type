#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "IListeningPort.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"

/**
 * @brief Module double listed after the network module of a server, which
 * writes the port that module listens on into a number the test keeps, once
 * the network module started; the number is left as it was when the server
 * does not reach its start.
 */
class ListeningPortRecordingModule final : public rtype::engine::IModule {
 public:
  /** @param port Where it writes the port; it outlives the module. */
  explicit ListeningPortRecordingModule(std::uint16_t& port);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  std::uint16_t* port_;
  const rtype::server::IListeningPort* listeningPort_{nullptr};
};

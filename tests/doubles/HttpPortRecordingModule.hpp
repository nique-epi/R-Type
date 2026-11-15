#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "MasterHttpServer.hpp"

/**
 * @brief Module double listed after the modules of the master, which writes
 * the port the HTTP server of the master listens on into a number the test
 * keeps, once that server started; the number is left as it was when the
 * master does not reach its start.
 */
class HttpPortRecordingModule final : public rtype::engine::IModule {
 public:
  /** @param port Where it writes the port; it outlives the module. */
  explicit HttpPortRecordingModule(std::uint16_t& port);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  std::uint16_t* port_;
  const rtype::master::MasterHttpServer* server_{nullptr};
};

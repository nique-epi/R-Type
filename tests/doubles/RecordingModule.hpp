#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "IModule.hpp"
#include "JournalModule.hpp"
#include "RecordingModuleSetup.hpp"

/** @brief Module double that writes each call it receives in a journal shared
 * with the other modules of the test; it opens nothing. */
class RecordingModule : public rtype::engine::IModule {
 public:
  /** @param journal Where the calls are written down; it must outlive the
   * module. */
  RecordingModule(RecordingModuleSetup setup, JournalModule& journal);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install() override;
  /** @throws StartRefusedException after writing the call down, when the setup
   * says the module fails to start. */
  void start() override;
  void stop() override;

 private:
  void record(std::string_view call);

  RecordingModuleSetup setup_;
  JournalModule* journal_;
};

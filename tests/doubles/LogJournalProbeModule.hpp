#pragma once

#include <string>
#include <vector>
#include "ILogJournal.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"

/** @brief Module double that looks up, during its install(), the journal the
 * logger module provides, and keeps what it found. It opens nothing. */
class LogJournalProbeModule : public rtype::engine::IModule {
 public:
  /** @param dependencies The names of the modules it is listed after. */
  LogJournalProbeModule(std::string name,
                        std::vector<std::string> dependencies);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

  /** @returns The journal found, or nullptr if no module provided one. */
  [[nodiscard]] rtype::logging::ILogJournal* journal() const;

 private:
  std::string name_;
  std::vector<std::string> dependencies_;
  rtype::logging::ILogJournal* journal_{nullptr};
};

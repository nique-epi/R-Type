#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"

/** @brief Module double that writes its name in a list when it is destroyed, so
 * a test reads the order in which an Application destroys its modules. It opens
 * nothing. */
class DestructionRecordingModule : public rtype::engine::IModule {
 public:
  /** @param destroyed Where the name is appended on destruction; it must
   * outlive the module. */
  DestructionRecordingModule(std::string name,
                             std::vector<std::string>& destroyed);
  DestructionRecordingModule(const DestructionRecordingModule&) = delete;
  DestructionRecordingModule& operator=(const DestructionRecordingModule&) =
      delete;
  DestructionRecordingModule(DestructionRecordingModule&&) = delete;
  DestructionRecordingModule& operator=(DestructionRecordingModule&&) = delete;
  ~DestructionRecordingModule() override;

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  std::string name_;
  std::vector<std::string>* destroyed_;
};

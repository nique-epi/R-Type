#pragma once

#include <string>
#include <vector>
#include "GameContext.hpp"
#include "IGameModule.hpp"
#include "InstallContext.hpp"

namespace rtype::server::testing {

/**
 * @brief A game module that writes down, in a journal the test owns, each
 * call it receives and each stage it runs in, each line starting with its name.
 * It can be told to throw from start().
 */
class RecordingGameModule final : public IGameModule {
 public:
  /**
   * @param name What the module is called, and how each journal line starts.
   * @param journal Where the lines are written; it must outlive the module.
   * @param failsToStart Whether start() throws.
   */
  RecordingGameModule(std::string name, std::vector<std::string>& journal,
                      bool failsToStart);

  [[nodiscard]] std::string name() const override;

  void install(GameContext& game, engine::InstallContext& install) override;

  void start() override;

  void stop() override;

 private:
  std::string name_;
  std::vector<std::string>* journal_;
  bool failsToStart_;
};

}  // namespace rtype::server::testing

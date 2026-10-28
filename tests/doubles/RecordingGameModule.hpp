#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "GameContext.hpp"
#include "IGameModule.hpp"
#include "InstallContext.hpp"

namespace rtype::server::testing {

/**
 * @returns The journal line of the module called @p module for @p what, a
 * call (INSTALL_CALL...), a stage of the tick or a player event: the module
 * name, a space, then @p what.
 */
[[nodiscard]] std::string journalLine(std::string_view module,
                                      std::string_view what);

/**
 * @brief A game module that writes down, in a journal the test owns, each
 * call it receives, each stage it runs in and each player event it is told
 * of, each line starting with its name. It can be told to throw from start().
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
  GameContext* game_{nullptr};
  std::string name_;
  std::vector<std::string>* journal_;
  bool failsToStart_;
};

}  // namespace rtype::server::testing

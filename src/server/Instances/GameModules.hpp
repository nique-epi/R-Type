#pragma once

#include <cstddef>
#include <memory>
#include <string_view>
#include <vector>
#include "IGameModule.hpp"
#include "InstallContext.hpp"
#include "LoopStages.hpp"
#include "ServiceRegistry.hpp"
#include "StageWork.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

struct GameContext;

/**
 * @brief The modules a game installed, the services they provide to each other
 * and the stages of the tick they add their work to.
 *
 * The modules are installed in the order they were listed, then started in
 * that order; the destructor stops the started ones in the opposite order. A
 * module whose start() threw is not stopped, and neither are the modules
 * after it, which never started. Not thread-safe: use it from the simulation
 * thread only.
 */
class GameModules {
 public:
  /**
   * @param modules Must hold no null pointer.
   */
  explicit GameModules(std::vector<std::unique_ptr<IGameModule>> modules);

  GameModules(const GameModules&) = delete;
  GameModules& operator=(const GameModules&) = delete;
  GameModules(GameModules&&) = delete;
  GameModules& operator=(GameModules&&) = delete;
  ~GameModules();

  /**
   * @brief Adds @p work to the stage @p stageName, after the work already
   * added to it.
   * @throws engine::UnknownStageException when no stage has that name.
   */
  void addWork(std::string_view stageName, engine::StageWork work);

  /**
   * @brief Installs every module with @p game, then starts them.
   * @throws engine::MissingServiceException when a module requires a service
   * no module before it provides.
   */
  void installAndStart(GameContext& game);

  /**
   * @returns The service of type @p Service that a module provided.
   * @throws engine::MissingServiceException when no module provided it.
   */
  template <typename Service>
  [[nodiscard]] Service& require() {
    return game_.require<Service>();
  }

  /**
   * @brief Runs the work of every stage, in the order of GAME_STAGES.
   */
  void runStages(engine::Duration elapsed);

 private:
  engine::ServiceRegistry services_;
  engine::LoopStages stages_;
  engine::InstallContext game_;
  std::vector<std::unique_ptr<IGameModule>> modules_;
  std::size_t startedCount_{0};
};

}  // namespace rtype::server

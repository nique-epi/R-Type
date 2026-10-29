#pragma once

#include <string>

namespace rtype::engine {

class InstallContext;

}  // namespace rtype::engine

namespace rtype::server {

struct GameContext;

/**
 * @brief A part of a game that installs itself in it: the rules of R-Type, the
 * replication of its world, or those of another game.
 *
 * A GameInstance installs its modules in the order they are listed, then starts
 * them in that order, when it is built, and stops the started ones in the
 * opposite order when it is destroyed. A module that needs the service of
 * another is listed after it.
 */
class IGameModule {
 public:
  virtual ~IGameModule() = default;

  /**
   * @returns What the module is called in a log line or an error.
   */
  [[nodiscard]] virtual std::string name() const = 0;

  /**
   * @brief Takes what the module needs from @p game, provides its services and
   * adds its work to the stages of the tick through @p install. It keeps
   * nothing open: what must live until stop() is opened in start().
   * @throws engine::MissingServiceException when it requires the service of a
   * module that is not installed before it.
   */
  virtual void install(GameContext& game, engine::InstallContext& install) = 0;

  /**
   * @brief Opens what the module keeps until stop(): subscriptions to the bus,
   * the parts that work on the world. A start() that throws releases what it
   * opened itself, since stop() is not called on a module whose start()
   * threw.
   */
  virtual void start() = 0;

  /**
   * @brief Lets go of everything start() opened, subscriptions included. The
   * game is about to be destroyed: no tick runs after it.
   */
  virtual void stop() = 0;
};

}  // namespace rtype::server

#pragma once

#include <string>
#include <vector>

namespace rtype::engine {

/**
 * @brief One part of a program that an Application installs, starts and stops.
 *
 * install() prepares the module and opens nothing; start() opens what the
 * module needs to run (a socket, a thread, a file); stop() closes it. An
 * Application calls install() once, start() once after every module is
 * installed, and stop() once after a start() that returned.
 */
class IModule {
 public:
  virtual ~IModule() = default;

  /** @returns The name other modules give to depend on this one. */
  [[nodiscard]] virtual std::string name() const = 0;

  /**
   * @returns The names of the modules this one depends on. Each must come
   * before it in the list given to the Application.
   */
  [[nodiscard]] virtual std::vector<std::string> dependencies() const = 0;

  /** @brief Prepares the module. Opens nothing. */
  virtual void install() = 0;

  /**
   * @brief Opens what the module needs to run.
   *
   * @throws std::exception, or a class derived from it, when the module cannot
   * start.
   */
  virtual void start() = 0;

  /**
   * @brief Closes what start() opened.
   *
   * Must not throw: the Application calls it while it undoes a failed start,
   * where a second error would hide the first, and from its destructor, where
   * an error ends the program.
   */
  virtual void stop() = 0;
};

}  // namespace rtype::engine

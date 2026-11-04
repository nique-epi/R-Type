#pragma once

#include <string>
#include <vector>

namespace rtype::engine {

/**
 * @brief What a module implements, besides IModule, when it uses the services
 * of other modules if they are listed and works without them otherwise.
 *
 * Its install() finds those services with InstallContext::find(), which sees
 * only the modules installed before it: the Application refuses a list where
 * one of them comes after it. Such a module may be left out of the list or
 * turned off, which the dependencies of IModule forbid.
 */
class IOptionalDependencies {
 public:
  virtual ~IOptionalDependencies() = default;

  /** @returns The names of the modules whose services this one uses when they
   * are listed; each listed one must come before it. */
  [[nodiscard]] virtual std::vector<std::string> optionalDependencies()
      const = 0;
};

}  // namespace rtype::engine

#pragma once

#include <span>
#include <string>
#include <vector>
#include "HandOverRule.hpp"

namespace rtype::engine {

/** @returns The rule that keeps the `ensure` lines of a configuration for
 * ensuredResources() instead of running them: a game reads which resources
 * it starts before it builds its modules. */
[[nodiscard]] HandOverRule ensureLineRule();

/**
 * @returns The resources the `ensure` lines of @p lines start, in the order of
 * their first line; a resource named twice is started once. Other lines are
 * skipped.
 * @throws InvalidEnsureLineException for an `ensure` line that does not name
 * exactly one resource, and InvalidResourceNameException for a name that is
 * not a resource name.
 */
[[nodiscard]] std::vector<std::string> ensuredResources(
    std::span<const std::string> lines);

}  // namespace rtype::engine

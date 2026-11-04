#pragma once

#include "AssetLoadAttempt.hpp"
#include "ClientException.hpp"

namespace rtype::client {

/** @brief Adds to @p problems what went wrong in @p attempt: the id when it
 * breaks the asset id rule, the file when it is missing or unreadable, nothing
 * when the asset was loaded. */
void recordProblem(AssetProblems& problems, const AssetLoadAttempt& attempt);

/** @returns Whether @p problems names any asset. */
[[nodiscard]] bool hasProblems(const AssetProblems& problems);

}  // namespace rtype::client

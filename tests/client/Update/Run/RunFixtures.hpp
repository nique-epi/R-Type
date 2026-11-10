#pragma once

#include <functional>

namespace rtype::client::testing {

/** @returns Whether @p condition held within THREAD_WAIT, asked every
 * THREAD_POLL: the check and the update run on threads of their own. */
[[nodiscard]] bool heldWithinTheWait(const std::function<bool()>& condition);

}  // namespace rtype::client::testing

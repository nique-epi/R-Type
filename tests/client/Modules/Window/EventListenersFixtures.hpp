#pragma once

#include <vector>
#include "EventListenerSlot.hpp"
#include "EventUse.hpp"

/**
 * @returns A listener that writes @p name at the end of @p seen each time it
 * is shown an event, then answers @p use. @p seen must outlive it.
 */
[[nodiscard]] rtype::client::EventListener recordingListener(
    std::vector<int>& seen, int name, rtype::client::EventUse use);

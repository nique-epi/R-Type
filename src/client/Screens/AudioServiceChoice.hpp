#pragma once

#include "IAudioService.hpp"
#include "InstallContext.hpp"

namespace rtype::client {

/**
 * @returns The audio service a module installed before provides, or
 * @p silence when no sound module is in the list: the client then runs without
 * sound, and no screen knows it.
 *
 * The sound module provides its service when it installs, so it must be
 * listed before the module that asks; clientModules() lists it before the
 * screens.
 */
[[nodiscard]] IAudioService& chooseAudioService(
    const engine::InstallContext& context, IAudioService& silence);

}  // namespace rtype::client

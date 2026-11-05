#pragma once

#include <span>
#include <string_view>
#include "ServerModeChoice.hpp"

namespace rtype::server {

/**
 * @brief Reads `--mode <server|supervisor|master>` when it is the first of
 * @p arguments, the arguments the binary was launched with, without its name.
 * @returns ServerMode::Server and every argument when the first is not
 * `--mode`, so a server launched as before runs as before; otherwise the mode
 * named and the arguments after it. A `--mode` further on is left to the
 * options of the mode, which refuse it.
 * @throws engine::MissingLaunchOptionValueException when `--mode` is the
 * last argument.
 * @throws engine::InvalidLaunchOptionValueException when its value is neither
 * SERVER_MODE_NAME, SUPERVISOR_MODE_NAME nor MASTER_MODE_NAME.
 */
[[nodiscard]] ServerModeChoice chooseServerMode(
    std::span<const std::string_view> arguments);

}  // namespace rtype::server

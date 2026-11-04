#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "FailureReport.hpp"
#include "IModule.hpp"
#include "LogLaunchOptions.hpp"
#include "ServerKeyOrder.hpp"

namespace rtype::master {

/**
 * @brief The modules of the master, in the order an Application installs and
 * starts them (and stops them in reverse): the logger, the storage, the keys
 * of the game servers, the HTTP API and the game servers. This is the one place
 * that lists them.
 *
 * The storage comes before the HTTP API, so a database the master cannot open
 * stops it before its port opens, and the API stops before the database
 * closes.
 *
 * @param logOptions Applied by the logger module when it starts.
 * @param onFailure Told by the HTTP module when its server stops on its own.
 * @param order The key the keys module issues when it starts, for
 * `--new-server-key`; empty for a master that runs.
 * @throws engine::UnknownExecutablePathException when the system does not
 * tell where the executable is, next to which the database lies by default.
 */
[[nodiscard]] std::vector<std::unique_ptr<engine::IModule>> masterModules(
    const logging::LogLaunchOptions& logOptions, const FailureReport& onFailure,
    std::optional<ServerKeyOrder> order);

/**
 * @returns The lines `--new-server-key` adds after the configuration: they
 * turn off `servers`, then `http`, which `servers` needs, so the master opens
 * no port while every setting of master.cfg stays known.
 */
[[nodiscard]] std::vector<std::string> keyIssueLines();

}  // namespace rtype::master

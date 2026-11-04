#pragma once

#include <memory>
#include <vector>
#include "FailureReport.hpp"
#include "IModule.hpp"
#include "LogLaunchOptions.hpp"

namespace rtype::master {

/**
 * @brief The modules of the master, in the order an Application installs and
 * starts them (and stops them in reverse): the logger, the storage and the
 * HTTP API. This is the one place that lists them.
 *
 * The storage comes before the HTTP API, so a database the master cannot open
 * stops it before its port opens, and the API stops before the database
 * closes.
 *
 * @param logOptions Applied by the logger module when it starts.
 * @param onFailure Told by the HTTP module when its server stops on its own.
 * @throws engine::UnknownExecutablePathException when the system does not
 * tell where the executable is, next to which the database lies by default.
 */
[[nodiscard]] std::vector<std::unique_ptr<engine::IModule>> masterModules(
    const logging::LogLaunchOptions& logOptions,
    const FailureReport& onFailure);

}  // namespace rtype::master

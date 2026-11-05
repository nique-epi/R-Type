#include "BoostProcessLauncher.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/system/system_error.hpp>
#include <memory>
#include <utility>
#include "BoostChildProcess.hpp"
#include "ChildBinding.hpp"
#include "IChildProcess.hpp"
#include "ProcessCommand.hpp"
#include "ServerException.hpp"

namespace rtype::server {

namespace {

/**
 * @returns The process @p command describes, started from @p context. On Linux
 * and macOS @p binding is an initializer of the launch; on Windows it joins the
 * process to its job afterwards (see ChildBinding).
 */
boost::process::v2::process startProcess(
    boost::asio::io_context& context, const ProcessCommand& command,
    [[maybe_unused]] ChildBinding& binding) {
#if defined(_WIN32)
  return boost::process::v2::process{context, command.program,
                                     command.arguments};
#else
  return boost::process::v2::process{context, command.program,
                                     command.arguments, binding};
#endif
}

}  // namespace

BoostProcessLauncher::BoostProcessLauncher() = default;

BoostProcessLauncher::~BoostProcessLauncher() = default;

std::unique_ptr<IChildProcess> BoostProcessLauncher::launch(
    ProcessCommand command) {
  try {
    boost::process::v2::process process =
        startProcess(context_, command, binding_);
    binding_.bind(process);
    return std::make_unique<BoostChildProcess>(std::move(process));
  } catch (const boost::system::system_error& error) {
    throw ProcessLaunchException(command.program.string(), error.what());
  }
}

}  // namespace rtype::server

#include "BoostProcessLauncher.hpp"
#include <array>
#include <boost/asio/readable_pipe.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/process/v2/stdio.hpp>
#include <boost/system/system_error.hpp>
#include <memory>
#include <utility>
#include <vector>
#include "BoostChildProcess.hpp"
#include "ChildBinding.hpp"
#include "ChildOutputRelay.hpp"
#include "IChildProcess.hpp"
#include "ProcessCommand.hpp"
#include "ProcessConstants.hpp"
#include "ServerException.hpp"

namespace rtype::server {

namespace {

/** @returns One relay per pipe of @p pipes, each reading it on its thread. */
std::vector<std::unique_ptr<ChildOutputRelay>> relaysOf(
    std::array<boost::asio::readable_pipe, CHILD_OUTPUTS>& pipes) {
  std::vector<std::unique_ptr<ChildOutputRelay>> relays;
  relays.reserve(pipes.size());
  for (boost::asio::readable_pipe& pipe : pipes) {
    relays.push_back(std::make_unique<ChildOutputRelay>(std::move(pipe)));
  }
  return relays;
}

}  // namespace

BoostProcessLauncher::BoostProcessLauncher() = default;

BoostProcessLauncher::~BoostProcessLauncher() = default;

std::unique_ptr<IChildProcess> BoostProcessLauncher::launch(
    ProcessCommand command) {
  try {
    std::array<boost::asio::readable_pipe, CHILD_OUTPUTS> pipes{
        boost::asio::readable_pipe{context_},
        boost::asio::readable_pipe{context_}};
    boost::process::v2::process process = startProcess(
        command, boost::process::v2::process_stdio{
                     .in = nullptr, .out = pipes[0], .err = pipes[1]});
    binding_.bind(process);
    return std::make_unique<BoostChildProcess>(std::move(process),
                                               relaysOf(pipes));
  } catch (const boost::system::system_error& error) {
    throw ProcessLaunchException(command.program.string(), error.what());
  }
}

boost::process::v2::process BoostProcessLauncher::startProcess(
    const ProcessCommand& command, boost::process::v2::process_stdio stdio) {
#if defined(_WIN32)
  return boost::process::v2::process{context_, command.program,
                                     command.arguments, std::move(stdio)};
#else
  return boost::process::v2::process{
      context_, command.program, command.arguments, std::move(stdio), binding_};
#endif
}

}  // namespace rtype::server

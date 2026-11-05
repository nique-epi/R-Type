#include "BoostProcessLauncher.hpp"
#include <boost/process/v2/process.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <memory>
#include <utility>
#include "BoostChildProcess.hpp"
#include "IChildProcess.hpp"
#include "ProcessCommand.hpp"
#include "ServerException.hpp"

namespace rtype::server {

BoostProcessLauncher::BoostProcessLauncher() = default;

BoostProcessLauncher::~BoostProcessLauncher() = default;

std::unique_ptr<IChildProcess> BoostProcessLauncher::launch(
    ProcessCommand command) {
  try {
    boost::process::v2::process process{context_, command.program,
                                        command.arguments, binding_};
    if (!binding_.lastProcessWasBound()) {
      boost::system::error_code ignored;
      process.terminate(ignored);
      throw ChildBindingException("AssignProcessToJobObject");
    }
    return std::make_unique<BoostChildProcess>(std::move(process));
  } catch (const boost::system::system_error& error) {
    throw ProcessLaunchException(command.program.string(), error.what());
  }
}

}  // namespace rtype::server

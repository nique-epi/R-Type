#include <fstream>
#include <memory>
#include <string>
#include <thread>
#include "BoostProcessLauncher.hpp"
#include "ChildProgramConstants.hpp"
#include "IChildProcess.hpp"
#include "ProcessCommand.hpp"

namespace {

/** The arguments of the parent: the file the child's process id is written
 * to, then the child program. */
constexpr int ARGUMENTS_NEEDED = 3;

}  // namespace

int main(int argumentCount, char** arguments) {
  if (argumentCount < ARGUMENTS_NEEDED) {
    return 2;
  }
  rtype::server::BoostProcessLauncher launcher;
  const std::unique_ptr<rtype::server::IChildProcess> child =
      launcher.launch(rtype::server::ProcessCommand{
          .program = arguments[2], .arguments = {std::string{SLEEP_COMMAND}}});
  std::ofstream{arguments[1]} << child->processId() << '\n';
  std::this_thread::sleep_for(CHILD_SLEEP_DURATION);
  return 0;
}

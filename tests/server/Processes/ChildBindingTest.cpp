#include <gtest/gtest.h>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include "BoostProcessLauncher.hpp"
#include "IChildProcess.hpp"
#include "ProcessCommand.hpp"
#include "ProcessFixtures.hpp"
#include "ProcessTestConstants.hpp"

using rtype::server::BoostProcessLauncher;
using rtype::server::IChildProcess;
using rtype::server::ProcessCommand;

/**
 * Given a program that started a child of its own through the launcher
 * When that program is killed
 * Then the child ends with it, so the program leaves no orphan behind
 */
TEST(ChildBinding, AChildDiesWhenTheProgramThatStartedItIsKilled) {
#if defined(__APPLE__)
  GTEST_SKIP() << "macOS has no call that ends a child with its parent: a game "
                  "process stops by itself when its supervisor is silent";
#endif
  const std::string processIdFile =
      (std::filesystem::temp_directory_path() / "rtype_orphan_test.pid")
          .string();
  std::filesystem::remove(processIdFile);
  BoostProcessLauncher launcher;
  const std::unique_ptr<IChildProcess> program = launcher.launch(
      ProcessCommand{.program = RTYPE_TEST_PARENT,
                     .arguments = {processIdFile, RTYPE_TEST_CHILD}});
  const std::optional<std::int64_t> found =
      waitForProcessIdIn(processIdFile, PROCESS_WAIT);
  ASSERT_TRUE(found.has_value());
  const std::int64_t grandChild = found.value_or(0);
  ASSERT_TRUE(processIsRunning(grandChild));

  program->terminate();

  EXPECT_TRUE(waitForProcessToEnd(grandChild, PROCESS_WAIT));
}

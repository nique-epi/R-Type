---
title: "Tutorial: write a module"
description: A step-by-step tutorial that adds a module to the server, from its folder to its tests, and runs it.
---

This tutorial adds a module to the server: a **tick report** that counts the ticks the server runs and writes the count in its journal every ten seconds. It is small on purpose. It touches every place a real module touches, in the order you will meet them, and nothing in it needs knowledge of the game. The concepts behind each step are on the [Modules](/R-Type/modules/) page.

Every file below was built and its tests were run on a copy of the repository before this page was written. You need the project to build (see the [README](https://github.com/nique-epi/R-Type#readme)) and a branch of your own started from `origin/main`.

## What you will have at the end

| Step | You add | In |
|---|---|---|
| 1 | The name of the module | `src/server/Modules/ModuleConstants.hpp` |
| 2 | The class and its constant | `src/server/Modules/TickReport/` |
| 3 | The library of the folder | `src/server/Modules/TickReport/CMakeLists.txt`, `src/server/Modules/CMakeLists.txt` |
| 4 | The module in the server's list | `src/server/Modules/Assembly/` |
| 5 | Three tests | `tests/server/Modules/TickReport/` |
| 6 | A run, and the three checks of the project | the terminal |

## Step 0: choose the place

Two questions decide where a module goes.

- **Which program?** The tick report writes in the journal and counts server ticks, so it belongs to the server. Anything that draws, plays a sound or reads a key belongs to the client (`src/client/Modules/`). The table "Where does a new class go?" in [Project layout](/R-Type/project-layout/#where-does-a-new-class-go) decides the other cases.
- **Which scope?** A module of the program (`IModule`) lives as long as the server runs. A module of a game (`IGameModule`) lives as long as one game, and there are several games at once. Counting server ticks is a matter of the program, so this is an `IModule`.

A module is one folder holding one library, named after what it does: `src/server/Modules/TickReport/`. The class is `ServerTickReportModule`, like `ServerLobbyModule` in `src/server/Modules/Lobby/`. Open that folder first: it is the smallest module that depends on others, and the model for this one.

## Step 1: give it a name

Other modules name a module to depend on it, so its name is a constant that both sides read. Add a line to `src/server/Modules/ModuleConstants.hpp`, after the lobby:

```cpp
constexpr std::string_view LOBBY_MODULE_NAME = "lobby";
constexpr std::string_view TICK_REPORT_MODULE_NAME = "tick-report";
```

## Step 2: write the class

Create `src/server/Modules/TickReport/` with three files.

The constant first. A number written in the code is named in a `<Concept>Constants.hpp` that holds constants only. `TickReportConstants.hpp`:

```cpp
#pragma once

#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::server {

/** @brief How many ticks pass between two lines of the tick report: ten
 * seconds of the simulation. */
constexpr std::size_t TICKS_PER_TICK_REPORT =
    10 * engine::SIMULATION_TICKS_PER_SECOND;

}  // namespace rtype::server
```

The name is not `TICKS_PER_REPORT` because the server already has a constant of that name (`src/server/ServerConstants.hpp`, for the lateness report of the tick loop). Two constants with one name in one namespace fail to compile with "redefinition". Search the repository for a name before you pick it: `git grep -n 'TICKS_PER_REPORT' -- src`.

The header, `ServerTickReportModule.hpp`. It implements `IModule` and declares each step; it holds no function body:

```cpp
#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"

namespace rtype::server {

/**
 * @brief The module that counts the ticks of the server and writes the count
 * in the journal every TICKS_PER_TICK_REPORT ticks.
 *
 * It depends on the logger module, so the journal is open when it writes. Its
 * work is added to the simulation stage in install(); start() sets the count to
 * zero and stop() writes the final count.
 */
class ServerTickReportModule final : public engine::IModule {
 public:
  ServerTickReportModule();

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /** @brief Adds the counting to the simulation stage; opens nothing. */
  void install(engine::InstallContext& context) override;

  /** @brief Sets the count to zero and logs that the counting begins. */
  void start() override;

  /** @brief Logs how many ticks were counted. */
  void stop() override;

  /** @returns How many ticks were counted since start(). */
  [[nodiscard]] std::size_t tickCount() const;

 private:
  void countTick();

  std::size_t tickCount_{0};
  logging::Logger logger_;
};

}  // namespace rtype::server
```

The source, `ServerTickReportModule.cpp`. Read it against the five moments:

```cpp
#include "ServerTickReportModule.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "ServerConstants.hpp"
#include "TickReportConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

ServerTickReportModule::ServerTickReportModule()
    : logger_(std::string{SERVER_LOGGER_NAME}) {}

std::string ServerTickReportModule::name() const {
  return std::string{TICK_REPORT_MODULE_NAME};
}

std::vector<std::string> ServerTickReportModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}};
}

void ServerTickReportModule::install(engine::InstallContext& context) {
  context.addWork(
      SIMULATION_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { countTick(); });
}

void ServerTickReportModule::start() {
  tickCount_ = 0;
  logger_.info("counting the ticks");
}

void ServerTickReportModule::stop() {
  logger_.info("counted ", tickCount_, " ticks");
}

std::size_t ServerTickReportModule::tickCount() const { return tickCount_; }

void ServerTickReportModule::countTick() {
  ++tickCount_;
  if (tickCount_ % TICKS_PER_TICK_REPORT == 0) {
    logger_.info(tickCount_, " ticks so far");
  }
}

}  // namespace rtype::server
```

| Function | Moment | Why it is written this way |
|---|---|---|
| Constructor | before any moment | The logger is built with the name the server's other lines carry (`SERVER_LOGGER_NAME`); nothing is opened |
| `name()`, `dependencies()` | when the `Application` is built | The module is listed after `logger`: the check refuses a list that puts it first |
| `install()` | install | It adds work to the `simulation` stage and opens nothing. The work captures `this`, which stays valid because the `Application` destroys the module after the loop stops |
| `start()` | start | It sets the count to zero, so a second start counts from zero. The logger module opened the journal in its own `start()`, which ran before |
| `countTick()` | run | It runs once per tick, in the `simulation` stage. It touches no service: it uses only its own members |
| `stop()` | stop | It writes the last count and does not throw |

The other stages are `inputs` and `outputs` (`SERVER_LOOP_STAGES`, `src/server/ServerConstants.hpp`). Work added to a stage runs in the order of the modules in the list, so the tick report, listed last, runs last in `simulation`.

## Step 3: declare its library

A folder holds one `CMakeLists.txt` and declares one library. `src/server/Modules/TickReport/CMakeLists.txt`, written on the model of the one in `Lobby/`:

```cmake
add_library(rtype_server_tick_report_module STATIC ServerTickReportModule.cpp)

target_include_directories(rtype_server_tick_report_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_tick_report_module
  PUBLIC
    rtype_engine_modules
    rtype_logging
  PRIVATE
    rtype_engine_time
    rtype_server_constants
    rtype_server_module_constants
)

rtype_enable_warnings(rtype_server_tick_report_module)
```

The libraries that the **header** names (`IModule`, `Logger`) are `PUBLIC`: whoever includes the header needs them. The ones only the `.cpp` and the constants use are `PRIVATE`. The server never links SFML, and the configure step stops if a library does.

The parent folder calls the new one. In `src/server/Modules/CMakeLists.txt`, before `Assembly`:

```cmake
add_subdirectory(Lobby)
add_subdirectory(TickReport)
add_subdirectory(Assembly)
```

## Step 4: put it in the server's list

`serverModules()` in `src/server/Modules/Assembly/ServerModules.cpp` is the one place that lists the modules of the server. Add the include and the line, last:

```cpp
#include "ServerNetworkModule.hpp"
#include "ServerTickReportModule.hpp"
```

```cpp
  modules.push_back(std::make_unique<ServerLobbyModule>());
  modules.push_back(std::make_unique<ServerTickReportModule>());
  return modules;
```

The module is only linked if its library is in the `CMakeLists.txt` of that folder. In `src/server/Modules/Assembly/CMakeLists.txt`, add it to the `PUBLIC` libraries of `rtype_server_modules`, in alphabetical order:

```cmake
    rtype_server_stop_request
    rtype_server_tick_report_module
```

Build the server now:

```bash
cmake --workflow --preset test
```

The first build of a branch takes a few minutes. When it ends, the server is listed with one module more, and one test fails: the one that checks the list of modules. That is the next step.

## Step 5: test it

Tests mirror the sources: the tests of `src/server/Modules/TickReport/` go in `tests/server/Modules/TickReport/`. Each test says what it checks in a Given / When / Then comment.

First, the existing test that lists the modules. In `tests/server/Modules/Assembly/ServerModulesTest.cpp`, add the new name at the end of the expected list, and add it to the sentence of the comment above the test:

```cpp
                       std::string{rtype::server::LOBBY_MODULE_NAME},
                       std::string{rtype::server::TICK_REPORT_MODULE_NAME}}));
```

Then `tests/server/Modules/TickReport/ServerTickReportModuleTest.cpp`. A module is tested inside a real `Application`, with the modules it depends on; `ServerModuleFixture` gives the logger options that write to a journal of the test only.

```cpp
#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "EngineException.hpp"
#include "IModule.hpp"
#include "LoopStages.hpp"
#include "ServerConstants.hpp"
#include "ServerLoggerModule.hpp"
#include "ServerModuleFixture.hpp"
#include "ServerTickReportModule.hpp"
#include "TickReportConstants.hpp"
#include "TimeConstants.hpp"

using rtype::engine::Application;
using rtype::engine::LoopStages;
using rtype::server::ServerLoggerModule;
using rtype::server::ServerTickReportModule;

class ServerTickReportModuleTest : public ServerModuleFixture {};

/**
 * Given a server application whose modules started
 * When the loop runs three passes
 * Then the tick report module counted three ticks
 */
TEST_F(ServerTickReportModuleTest, CountsOneTickPerPassOfTheLoop) {
  auto tickReport = std::make_unique<ServerTickReportModule>();
  const ServerTickReportModule& counter = *tickReport;
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<ServerLoggerModule>(journalOptions()));
  modules.push_back(std::move(tickReport));
  Application application(LoopStages(rtype::server::SERVER_LOOP_STAGES),
                          std::move(modules));
  application.start();
  const std::size_t passes = 3;

  for (std::size_t pass = 0; pass < passes; ++pass) {
    application.runStages(rtype::engine::SIMULATION_TICK_DURATION);
  }

  EXPECT_EQ(counter.tickCount(), passes);
}

/**
 * Given a server application whose modules started
 * When the loop runs TICKS_PER_TICK_REPORT passes
 * Then the journal holds the line of the report with that count
 */
TEST_F(ServerTickReportModuleTest, WritesTheCountInTheJournalEveryInterval) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<ServerLoggerModule>(journalOptions()));
  modules.push_back(std::make_unique<ServerTickReportModule>());
  Application application(LoopStages(rtype::server::SERVER_LOOP_STAGES),
                          std::move(modules));
  application.start();

  for (std::size_t pass = 0; pass < rtype::server::TICKS_PER_TICK_REPORT;
       ++pass) {
    application.runStages(rtype::engine::SIMULATION_TICK_DURATION);
  }

  const std::string expectedLine =
      std::to_string(rtype::server::TICKS_PER_TICK_REPORT) + " ticks so far";
  EXPECT_TRUE(std::ranges::any_of(
      journal().lines(), [&expectedLine](const std::string& line) {
        return line.find(expectedLine) != std::string::npos;
      }));
}

/**
 * Given the tick report module listed without the logger module it depends on
 * When the application is built
 * Then it is refused before anything is installed
 */
TEST_F(ServerTickReportModuleTest, IsRefusedWhenTheLoggerIsNotListedBeforeIt) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<ServerTickReportModule>());

  EXPECT_THROW(Application(LoopStages(rtype::server::SERVER_LOOP_STAGES),
                           std::move(modules)),
               rtype::engine::MissingDependencyException);
}
```

`Application::runStages()` is what the server's network thread calls once per tick, so a test that calls it three times plays three ticks without a socket or a clock.

Register the file in `tests/CMakeLists.txt`: add `server/Modules/TickReport/ServerTickReportModuleTest.cpp` to the `SOURCES` of `server_module_tests`, right after `server/Modules/Assembly/ServerModulesTest.cpp`. Its include folders come from `rtype_server_modules`, which links your library `PUBLIC`.

Run the tests of the module:

```bash
cmake --workflow --preset test
```

The suite ends green with no warning. To run only yours, once the build is done:

```bash
./build/tests/server_module_tests --gtest_filter='ServerTickReportModuleTest.*'
```

Three tests pass.

## Step 6: run it and check the project's gates

Start the server on a free port from the repository root, where `assets/waves/waves.toml` (`WAVE_FILE_PATH`) is found:

```bash
./r-type_server -p 4242
```

After ten seconds, stop it with Ctrl-C. The journal `r-type_server.log` holds the three lines of your module among the server's (each line starts with a time, left out here):

```text
[INFO ] [Server] - counting the ticks
[INFO ] [Server] - 600 ticks so far
[INFO ] [Server] - counted 720 ticks
```

The first line is `start()`, the second is the report at `TICKS_PER_TICK_REPORT` (10 s × 60 ticks per second), the third is `stop()`, written after the loop stopped, with the ticks run since. The count at stop depends on how long you left the server running.

Before opening a pull request, the three gates of the project:

```bash
cmake --workflow --preset test
cmake --build build --target format-check
cmake --build build --target tidy
```

`format` fixes the formatting for you (`cmake --build build --target format`). The conventions your files follow are in [CONTRIBUTING](https://github.com/nique-epi/R-Type/blob/main/CONTRIBUTING.md#code-structure): headers declare and sources define, no comment in a function body, a name written in full, a trailing underscore on a private member.

## What to change for other cases

| You want a module that… | Change |
|---|---|
| Uses a service of another module | In `install()`, `context.require<Service>()`, stored in a member; list the module that provides it in `dependencies()` |
| Offers a service to the modules after it | In `install()`, `context.provide<Service>(member_)`. The modules listed after it call `require<Service>()` |
| Opens a socket, a thread or a file | In `start()`, never in `install()`. If `start()` opens two things and the second fails, close the first before the exception leaves, because `stop()` is not called on a module whose `start()` threw |
| Reacts to an event | `bus.subscribe<Event>(…)` in `start()`, the `SubscriptionHandle` kept in a member, released in `stop()` |
| Can be turned off at launch | A launch option read in `LaunchOptions`, a value handed to the module's constructor, and a module that provides the same service doing nothing: see [Turning a module on or off](/R-Type/modules/#turning-a-module-on-or-off) |
| Belongs to the client | `src/client/Modules/`, listed in `clientModules()`, with the stages of `CLIENT_LOOP_STAGES` (`events`, `network`, `update`, `render`, `display`) |
| Is part of the rules of one game | An `IGameModule` listed in `rtypeGameModules()`: see [Server](/R-Type/server/#modules-of-a-game-igamemodule) |

## If it does not work

| You see | Cause | Fix |
|---|---|---|
| `redefinition of '…'` in your constants header | The name already exists in the namespace | Search it with `git grep`, pick another |
| `MissingDependencyException` naming your module and `logger` | The module is listed before the logger, or the logger is not in the list (a test that builds its own list) | List the dependency first |
| `MissingServiceException` naming your module | `require<>()` asked for a service no earlier module provides | List the providing module before yours, and add it to `dependencies()` |
| `UnknownStageException` | The stage name is not one of the program's | Use a constant of `SERVER_LOOP_STAGES`, never a literal |
| `undefined symbol` at the link of `r-type_server` | The library is not in the `PUBLIC` libraries of `rtype_server_modules` | Add it, as in step 4 |
| The server starts but your lines are missing | The module is not in `serverModules()`, or the journal is off (`--no-log-file`) | Check the list and the logging options |

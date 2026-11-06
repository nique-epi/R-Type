---
title: "Tutorial: write a module"
description: A step-by-step tutorial that adds a module to the server, from its folder to its tests, and runs it.
---

This tutorial adds a module to the server: a **tick report** that counts the ticks the server runs and writes the count in its journal every ten seconds. It is small on purpose. It touches every place a real module touches, in the order you will meet them, and nothing in it needs knowledge of the game. The concepts behind each step are on the [Modules](/R-Type/modules/) page.

Every file below was built and its tests were run on a copy of the repository before this page was written. You need the project to build (see the [README](https://github.com/nique-epi/R-Type#readme)) and a branch of your own started from `origin/main`.

## What you will have at the end

| Step | You add | In |
|---|---|---|
| 1 | The name of the module | `src/server/ModuleConstants.hpp` |
| 2 | The class and its constant | `src/server/TickReport/` |
| 3 | The library of the folder | `src/server/TickReport/CMakeLists.txt`, `src/server/CMakeLists.txt` |
| 4 | The module in the list of a game process, and its row in the list of modules | `src/server/Modules/`, `docs/src/content/docs/project-layout.md` |
| 5 | Three tests, their fixture and their constants | `tests/server/TickReport/` |
| 6 | A run, and the three checks of the project | the terminal |

## Step 0: choose the place

Three questions decide what your module is and where it goes.

- **Which program?** The tick report writes in the journal and counts server ticks, so it belongs to the server, and to the role that runs the ticks of a game: a game process, not the supervisor (see [Server](/R-Type/server/#the-three-roles-supervisor-game-and-master)). Anything that draws, plays a sound or reads a key belongs to the client (`src/client/`). The table "Where does a new class go?" in [Project layout](/R-Type/project-layout/#where-does-a-new-class-go) decides the other cases.
- **Which scope?** A module of the program (`IModule`) lives as long as the server runs. A module of a game (`IGameModule`) lives as long as one game, and there are several games at once. Counting server ticks is a matter of the program, so this is an `IModule`.
- **Required or optional?** The code that needs a required module may include it; an optional module is reached only through its `Contract/` sub-folder, and the program must work without it. The server does its job without a tick report and no other code needs it, so this one is optional, and it has no `Contract/` since nothing would go in it.

A module is one folder holding one CMake library, named after what it does: `src/server/TickReport/`. The class is `ServerTickReportModule`, like `ServerLobbyModule` in `src/server/Lobby/`. Open that folder first: its class is the smallest module that depends on others, and the model for this one, beside the `ConnectionAcceptor` it builds.

## Step 1: give it a name

Other modules name a module to depend on it, so its name is a constant that both sides read. Add a line to `src/server/ModuleConstants.hpp`, after the lobby:

```cpp
constexpr std::string_view LOBBY_MODULE_NAME = "lobby";
constexpr std::string_view TICK_REPORT_MODULE_NAME = "tick-report";
```

## Step 2: write the class

Create `src/server/TickReport/` with three files.

The constant first. A number written in the code is named in a `<Concept>Constants.hpp` that holds constants only, the ten seconds included. `TickReportConstants.hpp`:

```cpp
#pragma once

#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::server {

/** @brief Seconds between two lines of the tick report. */
constexpr std::size_t TICK_REPORT_SECONDS = 10;

/** @brief How many ticks pass between two lines of the tick report. */
constexpr std::size_t TICKS_PER_TICK_REPORT =
    TICK_REPORT_SECONDS * engine::SIMULATION_TICKS_PER_SECOND;

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
 * work is added to the simulation stage in install(); stop() writes the final
 * count.
 */
class ServerTickReportModule final : public engine::IModule {
 public:
  ServerTickReportModule();

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /** @brief Adds the counting to the simulation stage; opens nothing. */
  void install(engine::InstallContext& context) override;

  /** @brief Logs that the counting begins. */
  void start() override;

  /** @brief Logs how many ticks were counted. */
  void stop() override;

  /** @returns How many ticks were counted since the module was built. */
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

void ServerTickReportModule::start() { logger_.info("counting the ticks"); }

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
| `start()` | start | It writes a line, which needs the journal: the logger module opened it in its own `start()`, which ran before |
| `countTick()` | run | It runs once per tick, in the `simulation` stage. It touches no service: it uses only its own members |
| `stop()` | stop | It writes the last count and does not throw |

The other stages are `inputs` and `outputs` (`SERVER_LOOP_STAGES`, `src/server/ServerConstants.hpp`). Work added to a stage runs in the order of the modules in the list, so the tick report, listed after the lobby, runs after it in `simulation`.

## Step 3: declare its library

A folder holds one `CMakeLists.txt` and declares one library. `src/server/TickReport/CMakeLists.txt`, written on the model of the one in `src/server/Lobby/`:

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
)

rtype_enable_warnings(rtype_server_tick_report_module)
```

The libraries that the **header** names (`IModule`, `Logger`) are `PUBLIC`: whoever includes the header needs them. The ones only the `.cpp` and the constants use are `PRIVATE`. The server never links SFML, and the configure step stops if a library does.

The parent folder calls the new one. In `src/server/CMakeLists.txt`, before `Modules`:

```cmake
add_subdirectory(Metrics)
add_subdirectory(TickReport)
add_subdirectory(Modules)
```

## Step 4: put it in the list of a game process

`gameProcessModules()` in `src/server/Modules/GameProcessModules.cpp` is the one place that lists the modules of a game process; `supervisorModules()`, next to it, lists those of the supervisor, and a module added there is also named in `SUPERVISOR_MODULE_NAMES` (`ServerRoleConstants.hpp`). Add the include, and the line before the metrics module, so that its measure of a tick covers your work:

```cpp
#include "ServerNetworkModule.hpp"
#include "ServerTickReportModule.hpp"
```

```cpp
  modules.push_back(std::make_unique<ServerLobbyModule>());
  modules.push_back(std::make_unique<ServerTickReportModule>());
  modules.push_back(std::make_unique<ServerMetricsModule>(
      std::make_shared<engine::SystemClock>(), launchOptions.trafficCsvFile()));
  modules.push_back(std::make_unique<ServerSupervisorLinkModule>(
      launchOptions.supervisorLink(), stopRequest));
  return modules;
```

The module is only linked if its library is in the `CMakeLists.txt` of that folder. In `src/server/Modules/CMakeLists.txt`, add it to the `PUBLIC` libraries of `rtype_server_modules`, in alphabetical order:

```cmake
    rtype_server_platform
    rtype_server_tick_report_module
```

Every new module also adds its row to [the list of modules](/R-Type/project-layout/#the-list-of-modules), in the same pull request. In the table of a game process, after `lobby`:

```markdown
| `tick-report` | `ServerTickReportModule` | `src/server/TickReport/` | optional |
```

Build the server now:

```bash
cmake --workflow --preset test
```

The first build of a branch takes a few minutes. When it ends, a game process is listed with one module more, and one test fails: the one that checks the list of modules of a game process. That is the next step. A module that declares a setting also names it in `GAME_SETTING_NAMES` (`src/server/Modules/ServerRoleConstants.hpp`), so the supervisor hands its line of `server.cfg` to the games; `tests/server/Modules/ServerRolesTest.cpp` fails when it does not.

## Step 5: test it

Tests mirror the sources: the tests of `src/server/TickReport/` go in `tests/server/TickReport/`. Each test says what it checks in a Given / When / Then comment.

First, the existing test that lists the modules. In `tests/server/Modules/GameProcessModulesTest.cpp`, add the new name before the metrics in the expected list, and add it to the sentence of the comment above the test:

```cpp
                std::string{rtype::server::LOBBY_MODULE_NAME},
                std::string{rtype::server::TICK_REPORT_MODULE_NAME},
                std::string{rtype::server::METRICS_MODULE_NAME},
                std::string{rtype::server::SUPERVISOR_LINK_MODULE_NAME}}));
```

Then the tests of the module. A `…Test.cpp` holds only its tests: its constants and its fixture are files of their own, as in `tests/server/Lobby/` (`ServerLobbyFixture.{hpp,cpp}` and `ServerLobbyModuleTest.cpp`). A module is tested inside a real `Application`, with the modules it depends on; `ServerModuleFixture` gives the logger options that write to a journal of the test only.

`tests/server/TickReport/ServerTickReportModuleTestConstants.hpp`:

```cpp
#pragma once

#include <cstddef>

/** @brief How many passes of the loop the tests run to count a few ticks. */
constexpr std::size_t THREE_PASSES = 3;

/** @brief What follows the count in the line the report writes. */
constexpr const char* REPORT_LINE_ENDING = " ticks so far";
```

`ServerTickReportFixture.hpp` declares the base of the tests. It builds the application and keeps what the tests read:

```cpp
#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>
#include "Application.hpp"
#include "IModule.hpp"
#include "ServerModuleFixture.hpp"
#include "ServerTickReportModule.hpp"

/** @brief The base of the tests of the tick report module: an application
 * made of the logger module and the tick report module. */
class ServerTickReportFixture : public ServerModuleFixture {
 protected:
  /** @returns An application that lists the logger, then the tick report. */
  [[nodiscard]] rtype::engine::Application applicationWithTheTickReport();

  /** @returns A list holding the tick report module only. */
  [[nodiscard]] static std::vector<std::unique_ptr<rtype::engine::IModule>>
  tickReportWithoutTheLogger();

  /** @brief Runs @p passes passes of the loop of @p application, each lasting
   * one tick. */
  static void runPasses(rtype::engine::Application& application,
                        std::size_t passes);

  /** @returns How many ticks the tick report module of the application built
   * last has counted. */
  [[nodiscard]] std::size_t countedTicks() const;

  /** @returns Whether a line of the journal holds @p text. */
  [[nodiscard]] bool journalHolds(const std::string& text) const;

 private:
  const rtype::server::ServerTickReportModule* tickReport_{nullptr};
};
```

`ServerTickReportFixture.cpp` defines it:

```cpp
#include "ServerTickReportFixture.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "IModule.hpp"
#include "LoopStages.hpp"
#include "ServerConstants.hpp"
#include "ServerLoggerModule.hpp"
#include "ServerTickReportModule.hpp"
#include "TimeConstants.hpp"

rtype::engine::Application
ServerTickReportFixture::applicationWithTheTickReport() {
  auto tickReport = std::make_unique<rtype::server::ServerTickReportModule>();
  tickReport_ = tickReport.get();
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(
      std::make_unique<rtype::server::ServerLoggerModule>(journalOptions()));
  modules.push_back(std::move(tickReport));
  return rtype::engine::Application(
      rtype::engine::LoopStages(rtype::server::SERVER_LOOP_STAGES),
      std::move(modules));
}

std::vector<std::unique_ptr<rtype::engine::IModule>>
ServerTickReportFixture::tickReportWithoutTheLogger() {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<rtype::server::ServerTickReportModule>());
  return modules;
}

void ServerTickReportFixture::runPasses(rtype::engine::Application& application,
                                        std::size_t passes) {
  for (std::size_t pass = 0; pass < passes; ++pass) {
    application.runStages(rtype::engine::SIMULATION_TICK_DURATION);
  }
}

std::size_t ServerTickReportFixture::countedTicks() const {
  return tickReport_->tickCount();
}

bool ServerTickReportFixture::journalHolds(const std::string& text) const {
  return std::ranges::any_of(journal().lines(),
                             [&text](const std::string& line) {
                               return line.find(text) != std::string::npos;
                             });
}
```

`ServerTickReportModuleTest.cpp` holds the three tests, each with a Given / When / Then comment:

```cpp
#include <gtest/gtest.h>
#include <string>
#include "Application.hpp"
#include "EngineException.hpp"
#include "LoopStages.hpp"
#include "ServerConstants.hpp"
#include "ServerTickReportFixture.hpp"
#include "ServerTickReportModuleTestConstants.hpp"
#include "TickReportConstants.hpp"

using rtype::engine::Application;
using rtype::engine::LoopStages;

/**
 * Given a server application whose modules started
 * When the loop runs three passes
 * Then the tick report module counted three ticks
 */
TEST_F(ServerTickReportFixture, CountsOneTickPerPassOfTheLoop) {
  Application application = applicationWithTheTickReport();
  application.start();

  runPasses(application, THREE_PASSES);

  EXPECT_EQ(countedTicks(), THREE_PASSES);
}

/**
 * Given a server application whose modules started
 * When the loop runs TICKS_PER_TICK_REPORT passes
 * Then the journal holds the line of the report with that count
 */
TEST_F(ServerTickReportFixture, WritesTheCountInTheJournalEveryInterval) {
  Application application = applicationWithTheTickReport();
  application.start();

  runPasses(application, rtype::server::TICKS_PER_TICK_REPORT);

  EXPECT_TRUE(
      journalHolds(std::to_string(rtype::server::TICKS_PER_TICK_REPORT) +
                   REPORT_LINE_ENDING));
}

/**
 * Given the tick report module listed without the logger module it depends on
 * When the application is built
 * Then it is refused before anything is installed
 */
TEST_F(ServerTickReportFixture, IsRefusedWhenTheLoggerIsNotListedBeforeIt) {
  EXPECT_THROW(Application(LoopStages(rtype::server::SERVER_LOOP_STAGES),
                           tickReportWithoutTheLogger()),
               rtype::engine::MissingDependencyException);
}
```

`Application::runStages()` is what the server's network thread calls once per tick, so a test that calls it three times plays three ticks without a socket or a clock.

Register both `.cpp` files in a `tests/server/TickReport/CMakeLists.txt` of their own, `target_sources(server_module_tests PRIVATE ServerTickReportFixture.cpp ServerTickReportModuleTest.cpp)`, and add `add_subdirectory(TickReport)` to `tests/server/CMakeLists.txt`, where `server_module_tests` is declared. Its include folders come from `rtype_server_modules`, which links your library `PUBLIC`.

Run the tests of the module:

```bash
cmake --workflow --preset test
```

The suite ends green with no warning. To run only yours, once the build is done:

```bash
./build/tests/server_module_tests --gtest_filter='ServerTickReportFixture.*'
```

Three tests pass.

## Step 6: run it and check the project's gates

Start the supervisor from the repository root, where the build writes `r-type_server`, then a client, and CONNECT: the supervisor starts a game process for that player, and your module runs there.

```bash
./r-type_server -p 4242
```

After ten seconds, stop the supervisor with Ctrl-C; it stops its game. The journal of that game, `r-type_game_0.log` next to the executable for the first game the supervisor started, holds the three lines of your module among the game's (each line starts with a time, left out here):

```text
[INFO ] [Server] - counting the ticks
[INFO ] [Server] - 600 ticks so far
[INFO ] [Server] - counted 720 ticks
```

The first line is `start()`, the second is the report at `TICKS_PER_TICK_REPORT` (10 s × 60 ticks per second), the third is `stop()`, written after the loop stopped, with the ticks run since. The count at stop depends on how long the game process ran.

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
| Belongs to the client | `src/client/<Name>/`, listed in `clientModules()`, with the stages of `CLIENT_LOOP_STAGES` (`events`, `network`, `update`, `render`, `display`) |
| Is part of the rules of one game | An `IGameModule` in `src/server/<Name>/`, listed in `rtypeGameModules()`: see [Server](/R-Type/server/#modules-of-a-game-igamemodule) |
| Holds code another folder needs | Required: that folder includes it. Optional: the code goes in its `Contract/` sub-folder. See [The code of a module used elsewhere](/R-Type/project-layout/#the-code-of-a-module-used-elsewhere) |

## If it does not work

| You see | Cause | Fix |
|---|---|---|
| `redefinition of '…'` in your constants header | The name already exists in the namespace | Search it with `git grep`, pick another |
| `MissingDependencyException` naming your module and `logger` | The module is listed before the logger, or the logger is not in the list (a test that builds its own list) | List the dependency first |
| `MissingServiceException` naming your module | `require<>()` asked for a service no earlier module provides | List the providing module before yours, and add it to `dependencies()` |
| `UnknownStageException` | The stage name is not one of the program's | Use a constant of `SERVER_LOOP_STAGES`, never a literal |
| `undefined symbol` at the link of `r-type_server` | The library is not in the `PUBLIC` libraries of `rtype_server_modules` | Add it, as in step 4 |
| The server starts but your lines are missing | The module is not in `gameProcessModules()`, no player connected so no game process started, or you read `r-type_server.log`, the journal of the supervisor, instead of `r-type_game_<n>.log` | Check the list, connect a client, and read the journal of the game |

---
title: Modules
description: How a program is built from modules, what each module does at each moment of its life, how modules talk, and how to turn one on or off.
---

Both programs are built the same way: a list of **modules** that an `Application` installs, starts, runs and stops. A module is one part of a program with a single job: the logger, the network, the games, the lobby on the server; the window, the connection, the sound, the screens on the client. This page tells you what a module is, when its code runs, how it reaches the others and how to switch one off. To write one, follow the [tutorial](/R-Type/tutorial-write-a-module/).

The word has two meanings in this repository. A **module** on this page is a class implementing `IModule` (or `IGameModule`). A **module folder** in [Project layout](/R-Type/project-layout/#adding-a-class) is a folder with one CMake library. A module of this page usually lives in a module folder of its own, such as `src/server/Modules/Lobby/`, but the two are not the same thing.

The reasons for the design are in [Architecture](/R-Type/architecture/#modules). The classes are described in [Engine](/R-Type/engine/#modules-application-and-imodule).

## The five moments of a module

A module goes through five moments, always in this order. At each one, `Application` goes through the list from the first module to the last, except when stopping.

| Moment | Who calls it | What the module does | What it must never do |
|---|---|---|---|
| Read the configuration | `main()`, before any module exists | Nothing yet: the launch options are read once and handed to the constructors of the modules that need them (a port, a seed, `AudioMode`). A file a module reads itself (the settings, the waves) is read in its `install()` or its `start()` | Read the command line itself |
| Install | `Application::start()`, module after module | Builds its objects, finds the services of the modules before it, provides its own, adds its work to a loop stage; may read a file and close it | Bind a socket, start a thread, keep a file open, subscribe to an event |
| Start | `Application::start()`, once every module is installed | Opens what must live until it stops: a socket, a thread, a file, a subscription | Leave something open when it throws |
| Run | The program's loop, through `Application::runStages()` | Its work runs in the stage it was added to, once per tick or per frame, in list order within the stage | Look a service up: it uses the objects it got while installing |
| Stop | `Application::stop()`, in the reverse order | Closes everything `start()` opened | Throw |

What each moment guarantees follows from this table:

- A list is checked when the `Application` is built. A module whose dependency is not listed before it is refused with `MissingDependencyException`, and a null module with `NullModuleException`, before anything is installed.
- Installing opens nothing, so an installation that fails (`MissingServiceException`, `DuplicateServiceException`, `UnknownStageException`) leaves no socket or thread behind, and a module that was installed but never started is never stopped.
- When a `start()` throws, the modules started before it are stopped, the last started first, then `start()` throws `ModuleStartFailedException`, naming the module and the reason. The module that threw is not stopped, so its `start()` releases what it had opened before throwing.
- Stopping first stops the loop: the application waits for the pass in flight, and no work runs while the modules stop. A module is stopped once. The destructor of `Application` stops the modules still started, then destroys them from the last listed to the first, so a module outlives the modules that hold its services.

Which thread runs the loop belongs to a module too. On the server, the network module runs the tick on its thread and calls `runStages()` once per tick. On the client, the window module provides the `FrameLoop`, which `main()` runs on the main thread, where SFML reads the events.

## Two scopes: the program and the game

A module lives at one of two scopes.

| | Program | Game |
|---|---|---|
| Interface | `IModule` (`src/engine/Modules/IModule.hpp`) | `IGameModule` (`src/server/Instances/IGameModule.hpp`) |
| Owner | `Application`, one per program run | `GameInstance`, one per game hosted by the server |
| Lives | from the start of the program to its end | from the creation of the game to its destruction |
| Listed in | `serverModules()` (`src/server/Modules/Assembly/`), `clientModules()` (`src/client/Modules/Assembly/`) | `rtypeGameModules()` (`src/server/Modules/Games/RtypeGameModules.cpp`) |
| Stages | `SERVER_LOOP_STAGES`, `CLIENT_LOOP_STAGES` | `GAME_STAGES` |
| Example | the lobby, the sound | the rules of R-Type, the replication of the world |

The two follow the same five moments, the same order and the same rules. They differ in three ways:

- A game module has no `dependencies()`: it is listed after the modules whose services it requires, and a missing service throws `MissingServiceException` when the game is built.
- Its `install()` also receives a `GameContext` (the game's event bus, timers, players and settings), because several games run at once and each has its own.
- The program-scope `Application` runs the stages of the program; a game runs its own, three per tick, inside the work the games module adds to the simulation stage.

The server therefore has modules inside a module: the games module of the program creates the games, and each game installs its own modules. The client has the program scope only. The game scope is described in [Server](/R-Type/server/#modules-of-a-game-igamemodule).

## How modules talk

Modules never call each other directly and never include each other's headers. They meet through three channels.

| Channel | What it carries | Where it is set up |
|---|---|---|
| Services | An object a module hands to the modules installed after it | `provide<Service>()`, `require<Service>()`, `find<Service>()` on the `InstallContext`, during `install()` only |
| Stages | The order in which work runs | `addWork(stageName, work)`; the work of a stage runs in the order of the list |
| Events | Something that happened, for whoever wants to react | The `EventBus` of a game, reached through the `GameContext`; the hooks of a player (`SessionHooks`) |

A service is designated by its type and owned by the module that provides it. A second provider of the same type throws `DuplicateServiceException`. `require<>()` returns a reference or stops the installation, naming the module and the service; `find<>()` returns `nullptr` when the service is absent, which is how a module uses an optional service. The module then gives the object to its classes through their constructors: the context lives for the `install()` call only, so nothing is looked up while the loop runs.

Two lists say the same thing in two places, and both are checked:

- `dependencies()` names the modules that must be listed before. It is checked when the `Application` is built.
- `require<>()` names the services that must exist. It is checked while installing.

A module that requires a service lists the module that provides it in `dependencies()`. The server's lobby requires `Games`, `IDatagramSender`, `IPlayerAdmission` and `TickDatagrams`, so it depends on `games` and `network`.

An example of each channel, in the server today:

- The network module provides `IDatagramSender`; the games and lobby modules require it.
- The games module adds its work to the `simulation` stage; the lobby module adds its own to the same stage and runs after it, because it is listed after.
- A game's rules publish an event on its bus; the replication module subscribed to it in `start()` and sends the matching message to the players.

## Turning a module on or off

A module is switched on or off at one of three moments. Only the first two exist.

| How | Exists | How it works |
|---|---|---|
| At compile time | Yes | The module is in the list of `serverModules()` or `clientModules()`, and its library is linked by the `CMakeLists.txt` of that folder. Remove both lines to remove the module |
| At launch, by configuration | Yes | The module stays in the list; a launch option chooses what it does. `--no-sound` gives `ClientAudioModule` an `AudioMode::Disabled`: it still provides an `IAudioService`, one that plays nothing |
| At run time, by plugin | No | No module is loaded from a file while the program runs. A module is compiled into its program |

Two rules decide which one fits:

- **Take a module out of the list only when nothing depends on it.** Removing `audio` while `screens` requires an `IAudioService` stops the program before anything starts, with a message naming both modules. Take the dependents out with it.
- **Prefer configuration for a module others use.** A module that can be turned off keeps providing its service, with an implementation that does nothing, so the modules that use it never test whether it is on. The sound is the model: `SilentAudioService` answers every call and plays nothing, and the screens do not know which one they got.

Adding a launch option that turns a module off is three changes: the option in `LaunchOptions::read()` and `usage()` of the program (see [Server](/R-Type/server/#where-to-intervene) and [Client](/R-Type/client/#where-to-intervene)), a value the options hand to `clientModules()` or `serverModules()`, and the module's constructor taking it.

## Where to intervene

| You want to… | Look at |
|---|---|
| Add a part to the server | A new folder in `src/server/Modules/`, and a line in `serverModules()`: the [tutorial](/R-Type/tutorial-write-a-module/) does it step by step |
| Add a part to the client | A new folder in `src/client/Modules/`, and a line in `clientModules()` |
| Change the order modules start in | The order of the lines of `serverModules()` or `clientModules()`: a module comes after every module it depends on |
| Add a stage to a loop | `SERVER_LOOP_STAGES` (`src/server/ServerConstants.hpp`) or `CLIENT_LOOP_STAGES` (`src/client/ClientConstants.hpp`) |
| Add a rule to a game | An `IGameModule` listed in `rtypeGameModules()` |
| Give a module a name others can depend on | `src/server/Modules/ModuleConstants.hpp` or `src/client/Modules/ModuleConstants.hpp` |
| Change what happens when a module fails to start | `Application::startModules()` in `src/engine/Modules/Application/Application.cpp` |
| Test a module | An `Application` built from the module and the ones it depends on, as `tests/server/Modules/Logger/ServerLoggerModuleTest.cpp` does |

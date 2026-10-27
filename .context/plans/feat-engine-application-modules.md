# Plan: RTY-193, an engine application that installs, starts and stops modules

## Summary

The engine has no structure that tells a developer where a part of a program plugs in: the server and the client build their parts by hand in their `main.cpp`. This change adds to `rtype_engine` an `IModule` interface (a name, the modules it needs, `install()`, `start()`, `stop()`) and an `Application` class that installs then starts a list of modules in list order and stops them in reverse order, refusing a list whose required module is missing before anything is opened. No program uses it yet: the server and the client do not change.

The design follows the module system document the issue links ("Système de modules R-Type", 26 October 2026, read from the copy the user attached; see Assumptions). This change is the first half of its step 1, "Squelette dans le moteur", which Linear splits into RTY-193 and RTY-194. Its "Décisions" section settled that dynamic plugins will exist (RTY-205, RTY-208) and will implement this same module interface, so the interface is written to be loadable from a shared library unchanged (decision D10). The loader itself is not part of this change.

Issue: **RTY-193**. The pull request delivers the whole issue: `Closes RTY-193`.

## Acceptance criteria

| # | Criterion (from the issue) | How it is met | Checked by |
|---|---|---|---|
| 1 | `rtype_engine` provides an `Application` class and a pure virtual module interface; a module declares its name and the modules it needs, then installs, starts and stops | `src/engine/Modules/IModule.hpp` (pure virtual: `name()`, `requiredModules()`, `install()`, `start()`, `stop()`), `src/engine/Modules/Application/` (`rtype_engine_application`, linked into `rtype_engine`) | build; `ApplicationTest.cpp` uses both through `rtype_engine` |
| 2 | Modules install then start in the order of the list the program gives, and stop in reverse order | `start()` installs every module in list order, then starts every module in list order; `stop()` and the destructor stop the started modules last first | `InstallsEveryModuleThenStartsEveryModuleInListOrder`, `StopsModulesInReverseListOrder`, `DestructionStopsStartedModulesInReverseListOrder` |
| 3 | A missing required module stops the program before anything is opened (socket, thread, file), through an engine exception that names the missing module | the constructor checks the list and throws `MissingRequiredModuleException` (derives from `EngineException`); no module method other than `name()` and `requiredModules()` runs before `start()` | `RefusesAModuleWhoseRequiredModuleIsMissing`, `MissingRequiredModuleErrorNamesTheMissingModule`, `MissingRequiredModuleCallsNoModule`, `RefusesARequiredModuleListedAfterTheModuleThatNeedsIt` |
| 4 | When a start fails, the modules already started stop in reverse order, then the error goes up with the name of the failing module | `startModules()` catches the `std::exception`, calls `stop()`, then throws `ModuleStartFailedException(name, reason)` | `FailedStartStopsTheStartedModulesInReverseListOrder`, `FailedStartErrorNamesTheFailingModule`, `StopAfterAFailedStartStopsNothing` |
| 5 | No R-Type word in this code; no program changes | the new code names modules, an application and a journal only; `src/server`, `src/client`, `src/game`, `src/network` untouched | `git grep -n -i -E 'r-?type|bydo|missile|score|ship' -- src/engine/Modules` prints only `namespace rtype::engine` lines; `git diff --stat origin/main...HEAD -- src/server src/client src/game src/network` is empty |
| 6 | Given / When / Then tests: install and start order, reverse stop, missing dependency, failed start | 15 tests in `tests/engine/Modules/ApplicationTest.cpp` | `ctest --preset test -R '^Application\.'` |
| 7 | `cmake --workflow --preset test` green, zero warnings | full build from a clean configure, log read | Step 6 |

## Out of scope

- Using `Application` in the server or the client: the issue says no program changes. Their `main.cpp` keep assembling their parts by hand.
- The other moments of the document's skeleton. In "Le squelette d'application et ses cinq moments", this change covers 2 (install), 3 (start) and 5 (stop). Moment 1 (read the configuration, `server.cfg`) is RTY-195. Moment 4 (run: the stages Entrées / Simulation / Sorties on the server, Événements / Réseau / Mise à jour / Rendu on the client) is not in the issue.
- Anything a module installs *into*: the installation context, the services, the stages and the event subscriptions. The document lists them in step 1 next to `Application` and the module interface. Since RTY-193's own text names only the application, the interface and the lifecycle, they are taken to be RTY-194 (assumption). `install()` therefore takes no parameter in this step; RTY-194 gives it its context.
- The dynamic plugin loader, its C entry point and its API version number (RTY-205), and the demonstration plugin `stats` (RTY-208). D10 lists what this change guarantees them.
- The two scopes "programme" and "partie". On 26 October, the document settled on one process per game (decision 6, and "Un thread ou un processus par partie"). Under that model, "le choix d'un rôle se réduit donc au choix d'une liste de modules" (choosing a role comes down to choosing a list of modules), so the engine needs no scope concept: one `Application` per process.
- Sorting modules by their requirements. The document rules it out ("Ce qu'on ne fait pas": "Pas de graphe de dépendances résolu tout seul […] L'ordre, c'est la liste de `main.cpp`"); see D3.
- Restarting an application after `stop()`: an application starts once (decision D5).
- Duplicate module names: not checked. With two modules of the same name, a requirement is met by the first one listed before the module that needs it.
- The order in which the module objects are destroyed. `std::vector` destroys them after `stop()`, and no module holds a reference to another in this step. Once RTY-194 lets a module keep another's service by reference, and RTY-205 unloads a plugin library after its module, the `Application` destructor will need to destroy the modules last listed first. That belongs with RTY-194.

  Two measurements say why it is not done here:
  - libc++ already destroys a `std::vector` from its last element (measured with Apple clang 21.0.0), so a test of that order could not turn red locally.
  - A test double that records its own destruction must write into a journal, and clang-tidy's `bugprone-exception-escape` reports a destructor that appends to a `std::vector` (measured).
- Moving the older tests at the root of `tests/` (`SystemSchedulerTest.cpp`, …) into `tests/engine/`: pre-existing, reported in the pull request, not touched.

## Areas of the code concerned

| Path | Change |
|---|---|
| `src/engine/Modules/IModule.hpp` | **new**: the module interface |
| `src/engine/Modules/CMakeLists.txt` | **new**: `rtype_engine_modules` (INTERFACE, exposes `IModule.hpp`), `add_subdirectory(Application)` |
| `src/engine/Modules/Application/Application.{hpp,cpp}` | **new**: the `Application` class |
| `src/engine/Modules/Application/CMakeLists.txt` | **new**: `rtype_engine_application` (STATIC) |
| `src/engine/CMakeLists.txt` | `add_subdirectory(Modules)`; `rtype_engine_modules` and `rtype_engine_application` added to `rtype_engine` |
| `src/engine/Exceptions/EngineException.{hpp,cpp}` | three exceptions added: `NullModuleException`, `MissingRequiredModuleException`, `ModuleStartFailedException` |
| `tests/doubles/RecordingModule.{hpp,cpp}`, `ModuleJournal.hpp`, `RecordingModuleSetup.hpp`, `RecordingModuleConstants.hpp`, `StartRefusedException.{hpp,cpp}` | **new**: the module double and what it uses |
| `tests/engine/Modules/ApplicationTest.cpp`, `ApplicationTestConstants.hpp`, `ApplicationFixtures.{hpp,cpp}` | **new**: the tests |
| `tests/CMakeLists.txt` | the new sources added to `engine_tests`, `tests/doubles` added to its include folders |
| `docs/src/content/docs/engine.md`, `architecture.md`, `project-layout.md` | documentation (Step 7) |

Placement, rule by rule:

- `IModule.hpp` is an interface, header-only, so it sits in the parent folder that declares the target exposing it (`architecture-one-folder-per-concrete-class.md` point 3), exactly as `src/engine/Systems/ISystem.hpp` sits next to `Systems/SystemScheduler/`.
- `Application` is a concrete class with a `.cpp`: its own folder `Modules/Application/` with its `CMakeLists.txt` (`architecture-one-folder-per-concrete-class.md`, `fix-architecture-a-cpp-hpp-pair-has-its-own-folder.md`, `fix-architecture-one-cmakelists-per-folder.md`).
- The project map (`docs/src/content/docs/project-layout.md:59-74`) lists no folder for modules; it says what `src/engine` holds ("the building blocks of a networked game, R-Type or not") and that the engine never holds an R-Type word, which this code respects. Step 7 adds `Modules/` to the map's tree.
- The exceptions go in the engine's only exceptions file, `src/engine/Exceptions/EngineException.{hpp,cpp}`, like every other engine error (`architecture-custom-exceptions-per-module.md`).
- `RecordingModule` stands in for a production interface: `tests/doubles/` (`fix-process-read-the-project-map-before-placing-a-folder.md`, "A test double follows the map"). Its journal, setup, constants and exception sit next to it, one type per header.
- `ApplicationTest.cpp` tests `src/engine/Modules/Application/`: it goes in `tests/engine/Modules/`, the way `tests/engine/Time/FixedTimestepTest.cpp` tests `src/engine/Time/FixedTimestep/` (the class folder level is not repeated). Its constants and fixtures sit next to it (`fix-format-a-test-file-holds-only-tests.md`).
- Links: `rtype_engine_application` links `rtype_engine_modules` and `rtype_engine_exceptions`, neither of which links anything in `Modules/`, so no link points back (`fix-architecture-a-link-never-points-back-at-a-folder-that-links-it.md`). `rtype_forbid_links(rtype_engine "SFML::" "asio::" "rtype_game" "rtype_network" "rtype_client")` (`CMakeLists.txt:73`) still holds.

## Design decisions

The decisions come from the issue, the module system document it links, and the code. "Document" below means that page. Its quotes are in French, as written; what is concluded from them is said apart from the quotes. The document calls its class names "indicatifs" (indicative): `Application` and "l'interface de module" are its words, and `IModule` follows the repository's `I` prefix for interfaces.

| # | Decision | Why |
|---|---|---|
| D1 | `IModule` has `name()` and `requiredModules()`, returning `std::string` and `std::vector<std::string>` | The document's definition: a module "déclare son nom et les modules dont elle a besoin, puis elle s'installe, démarre et s'arrête". Names are compared once, when the list is checked. Owned strings have no lifetime to explain, unlike a `std::string_view` into a module, which matters once that module lives in a plugin library (D10) |
| D2 | The list is given once, to the constructor, as `std::vector<std::unique_ptr<IModule>>`, and checked there | Document: "Le `main.cpp` lui donne la liste des modules du programme". A list that cannot change after construction has no "add after start" case, and a missing module throws before `start()`, so before anything can be opened |
| D3 | A required module must be listed **before** the module that needs it; listed after it, it counts as missing | Document, "Ce qu'on ne fait pas": "Le squelette vérifie seulement que les dépendances déclarées sont présentes et placées avant." One check covers "absent" and "too late", and the message says "not listed before it" |
| D4 | `start()` installs every module first, then starts every module | Document: every module goes through the same moments "dans l'ordre de la liste". Installing is when each module registers "ses services, son travail dans les étapes, ses commandes et ses abonnements" and "rien ne s'ouvre encore"; starting is when "chaque module ouvre ce qui le concerne". The document draws these as moments of the whole program, so every module is installed before any starts. This is also the order of `IAppSystem.h`, which the issue cites |
| D4b | `IModule` has no `update()` or per-tick method | Document: "Pourquoi des étapes plutôt qu'une méthode `update()` par module": a module with nothing to do each tick writes nothing, and the interface stays purely virtual, with no empty methods. Per-tick work is registered in stages at install time (RTY-194) |
| D5 | `start()` acts once: a second call, even after `stop()`, does nothing | `fix-reasoning-a-sequence-asserts-its-starting-state.md`: a repeated call must not install twice. Measured: without this guard, a second `start()` leaves the started count above the number of modules and `stop()` reads past the end of the list (the red proof crashes) |
| D6 | `stop()` stops the started modules last first, each once; the destructor calls it | a program that leaves through an exception still stops its modules; a second `stop()` is harmless |
| D7 | A module throwing from `start()`: the started modules stop in reverse order, then `ModuleStartFailedException(name, reason)` is thrown; the module that threw is not stopped | Document: "Si un démarrage échoue, les modules déjà démarrés s'arrêtent dans l'ordre inverse, puis le programme sort avec le nom du module fautif." Making the program exit is its `main.cpp`'s job, once it catches the exception. The module that threw did not start, so it has nothing to stop. The reason is the original exception's `what()`, so the message says what failed |
| D8 | `IModule::stop()` must not throw, said in its doc comment, **not** marked `noexcept` | measured: clang-tidy 20's `bugprone-exception-escape` reports a `noexcept` function that appends to a `std::vector` (libc++), so a `noexcept` interface would make every real `stop()` that allocates fail the gate. The destructor calling it is not reported (measured) |
| D9 | `install()` errors go through unchanged | The issue only names failed starts. `install()` opens nothing, so no module has anything to stop at that point |
| D10 | The interface can be implemented by a dynamic plugin unchanged; nothing here loads one | Document, decision 1, settled on 26 October: dynamic plugins, "même interface que les modules liés, côté serveur seulement, chargés au lancement et jamais déchargés". Under "Le plugin dynamique": "Un plugin implémente l'interface de module ordinaire et exporte une seule fonction C qui la crée, avec un numéro de version d'API. […] Un module ne sait pas s'il a été lié ou chargé." See "What the plugins need from this change" below |

### What the plugins need from this change

The plugin loader and its demonstration are RTY-205 and RTY-208, in the document's step 6. Each point below is a property of this change, set against the sentence of the document it serves.

| The document says | This change |
|---|---|
| "Un plugin implémente l'interface de module ordinaire" | `IModule` is pure virtual, with no data member and no default body (`architecture-pure-virtual-interfaces.md`), so a class in a shared library can implement it. `Application` holds `std::unique_ptr<IModule>` and never names a concrete module type |
| "Un module ne sait pas s'il a été lié ou chargé" | Nothing in `IModule` or `Application` tells the two apart: no flag, no origin, no plugin-specific method |
| "exporte une seule fonction C qui la crée, avec un numéro de version d'API" | Belongs to the loader (RTY-205): the entry point and the version constant are not declared here, so this change does not fix their names before that issue designs them |
| "Même compilateur, même version du moteur. Le format binaire du C++ change d'un compilateur à l'autre" | `name()` and `requiredModules()` return `std::string` and `std::vector<std::string>` across the interface. That is safe only under this condition of the document, which RTY-205 enforces with its API version |
| "Chargé au lancement, déchargé seulement à la sortie, une fois le module détruit" | `Application` owns its modules and destroys them in its own destructor, after `stop()`. A loader built before the `Application` and destroyed after it therefore unloads a library only once its module is gone. The order between modules is for RTY-194 (Out of scope) |
| "Risques à tester d'abord […] `std::type_index`" | `Application` uses no `std::type_index`. It catches a failed start as `const std::exception&`, which relies on matching the exception's type across the library boundary. Whether a `std::exception` thrown inside a plugin is caught there on Linux, macOS and Windows is for RTY-205 to test, as the document asks for its other risks |

## Test strategy

GoogleTest, suite `Application`, in `engine_tests`, one behavior per test, each with its Given / When / Then comment (`code-style-tests-given-when-then.md`). The double is `RecordingModule`: it writes each call it receives (`install first`, `start second`, …) into a `ModuleJournal` shared by the modules of one test, so a test reads the order of the calls across modules. A module set to fail writes `start <name>` then throws `StartRefusedException`, a `std::runtime_error` that stands for any error a real module throws.

| Kind | Test | Expected value comes from |
|---|---|---|
| nominal | `InstallsEveryModuleThenStartsEveryModuleInListOrder` | the issue: install, then start, in list order |
| nominal | `StopsModulesInReverseListOrder` | the issue: reverse order |
| nominal | `DestructionStopsStartedModulesInReverseListOrder` | D6 |
| bound (repeat) | `SecondStopStopsNothing`, `SecondStartDoesNothing` | D5, D6 |
| bound (empty) | `ApplicationWithoutModulesStartsAndStops` | `fix-reasoning-bounded-contract-check-the-empty-state.md` |
| error | `RefusesAModuleWhoseRequiredModuleIsMissing`, `MissingRequiredModuleErrorNamesTheMissingModule`, `MissingRequiredModuleCallsNoModule` | the issue: engine exception naming the module, before anything opens |
| bound (order) | `RefusesARequiredModuleListedAfterTheModuleThatNeedsIt`, `AcceptsARequiredModuleListedBeforeTheModuleThatNeedsIt` | D3, both sides of the bound |
| error | `RefusesANullModule` | the convention of `SystemScheduler::add` (`NullSystemException`) |
| error | `FailedStartStopsTheStartedModulesInReverseListOrder`, `FailedStartErrorNamesTheFailingModule`, `StopAfterAFailedStartStopsNothing` | the issue: rollback, then the error with the module's name; D7 |

The two message tests compare the whole message with a constant written from the rule ("names the module and the missing module", "names the module and gives the reason"), as `WaveCatalogTest.cpp:114` does.

**Red proofs, measured during planning** on a scratch copy outside the repository (the plan's code, built with `/usr/bin/c++` Apple clang 21.0.0 and GoogleTest from `build/vcpkg_installed`): the original passes 15/15; each injected defect turns these tests red:

| Defect injected | Tests red |
|---|---|
| `stop()` walks the list forward | `StopsModulesInReverseListOrder`, `DestructionStopsStartedModulesInReverseListOrder`, `FailedStartStopsTheStartedModulesInReverseListOrder` |
| each module installed then started in turn | `InstallsEveryModuleThenStartsEveryModuleInListOrder`, `FailedStartStopsTheStartedModulesInReverseListOrder`, `FailedStartErrorNamesTheFailingModule`, `StopAfterAFailedStartStopsNothing` |
| no `stop()` when a start fails | `FailedStartStopsTheStartedModulesInReverseListOrder`, `StopAfterAFailedStartStopsNothing` |
| no check of the required modules | `RefusesAModuleWhoseRequiredModuleIsMissing`, `MissingRequiredModuleErrorNamesTheMissingModule`, `MissingRequiredModuleCallsNoModule`, `RefusesARequiredModuleListedAfterTheModuleThatNeedsIt` |
| required module searched in the whole list | `RefusesARequiredModuleListedAfterTheModuleThatNeedsIt` |
| no once-only guard in `start()` | the test binary crashes (SIGSEGV) in `SecondStartDoesNothing`: the destructor reads past the end of the list |
| destructor stops nothing | `DestructionStopsStartedModulesInReverseListOrder` |
| `stop()` does not lower the started count | `SecondStopStopsNothing`, `StopAfterAFailedStartStopsNothing` |
| start error message without the reason | `FailedStartErrorNamesTheFailingModule` |
| missing-module message without the missing name | `MissingRequiredModuleErrorNamesTheMissingModule` |

Not proven red: `RefusesANullModule` (without the check, a null module is dereferenced: undefined behavior, not a reliable red), `AcceptsARequiredModuleListedBeforeTheModuleThatNeedsIt` and `ApplicationWithoutModulesStartsAndStops` (they guard against a check that refuses too much; no variant above does). The red proofs are not part of the delivered work; the executing step reruns the suite only.

## Implementation steps

Each step names its files; all the code below was formatted with `clang-format --style=file --assume-filename=<its path>` on standard input from the repository root (Homebrew clang-format 20.1.8; the CI runs clang-format 20), compiled with `-std=c++20 -Wall -Wextra -Wpedantic -Werror` (Apple clang 21.0.0), and linted with the repository's `.clang-tidy` (Homebrew clang-tidy 20.1.8, libc++, headers linted as main files, calibrated on a planted `int* pointer = 0;` that it reports): 0 findings. GCC and MSVC were not run.

### Step 1 — The module interface: `src/engine/Modules/IModule.hpp`

**Why.** A developer needs one shape for "a part of the program" so they know where to plug a feature in, and can take it out without touching the rest.

**What we do.** Add the pure virtual interface `IModule`: a name, the names of the modules it needs, and three steps.

**What it does.** Nothing by itself: it is the contract `Application` calls. `install()` prepares and opens nothing; `start()` opens what the module needs (a socket, a thread, a file) and reports a failure by throwing a `std::exception`; `stop()` closes it and must not throw.

**How it is used.** A module of a program (illustration: no such module exists in this change; the tests' `RecordingModule`, Step 4, is the only implementation):

```cpp
class AudioModule : public rtype::engine::IModule {
 public:
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> requiredModules() const override;
  void install() override;
  void start() override;
  void stop() override;
};

std::string AudioModule::name() const { return std::string(AUDIO_MODULE); }

std::vector<std::string> AudioModule::requiredModules() const {
  return {std::string(LOG_MODULE)};
}
```

```cpp
#pragma once

#include <string>
#include <vector>

namespace rtype::engine {

/**
 * @brief One part of a program that an Application installs, starts and stops.
 *
 * install() prepares the module and opens nothing; start() opens what the
 * module needs to run (a socket, a thread, a file); stop() closes it. An
 * Application calls install() once, start() once after every module is
 * installed, and stop() once after a start() that returned.
 */
class IModule {
 public:
  virtual ~IModule() = default;

  /** @returns The name other modules give to require this one. */
  [[nodiscard]] virtual std::string name() const = 0;

  /**
   * @returns The names of the modules this one needs. Each must come before it
   * in the list given to the Application.
   */
  [[nodiscard]] virtual std::vector<std::string> requiredModules() const = 0;

  /** @brief Prepares the module. Opens nothing. */
  virtual void install() = 0;

  /**
   * @brief Opens what the module needs to run.
   *
   * @throws std::exception, or a class derived from it, when the module cannot
   * start.
   */
  virtual void start() = 0;

  /**
   * @brief Closes what start() opened.
   *
   * Must not throw: the Application calls it while it undoes a failed start,
   * where a second error would hide the first, and from its destructor, where
   * an error ends the program.
   */
  virtual void stop() = 0;
};

}  // namespace rtype::engine
```

**Reading the code, symbol by symbol.**

- `name()`: returns the name other modules write in their `requiredModules()`. Read by `Application`'s check (`listsModule`) and by the error messages.
- `requiredModules()`: returns the names this module needs; each must belong to a module listed before it (D3). Read once, by the constructor of `Application`.
- `install()`, `start()`, `stop()`: called by `Application` only, in the order of D4 to D7. `stop()` is not `noexcept` (D8).
- No data member, `= default` virtual destructor only: `architecture-pure-virtual-interfaces.md`. `cppcoreguidelines-special-member-functions` accepts the lone defaulted destructor (`AllowSoleDefaultDtor: true` in `.clang-tidy`), as for `ISystem`.

### Step 2 — The three engine errors: `src/engine/Exceptions/EngineException.{hpp,cpp}`

**Why.** A missing module and a failed start must reach the program as engine errors that say which module, so the program can print a clear message, and a `catch (const EngineException&)` catches them with the others.

**What we do.** Add three classes to the end of the engine's exceptions file, after `InvalidLaunchOptionValueException`, each building its message from its parameters.

**What it does.** `NullModuleException`: "A null module cannot be given to an application". `MissingRequiredModuleException(moduleName, requiredModule)`: `The module "<moduleName>" requires the module "<requiredModule>", which is not listed before it`. `ModuleStartFailedException(moduleName, reason)`: `The module "<moduleName>" failed to start: <reason>`.

**How it is used.** Thrown by `Application` (Step 3: `Application.cpp`, `requireNoNullModule`, `requireModulesListedBefore`, `startModules`); a program would catch them as it catches the launch errors (illustration, no program does it in this change):

```cpp
try {
  rtype::engine::Application application(programModules());
  application.start();
  runUntilClosed();
} catch (const rtype::engine::EngineException& error) {
  std::cerr << error.what() << '\n';
  return EXIT_FAILURE;
}
```

`EngineException.hpp`, added before the closing `}  // namespace rtype::engine` (the rest of the file is unchanged):

```cpp
/**
 * @brief A null module was given to an application.
 */
class NullModuleException : public EngineException {
 public:
  NullModuleException();
};

/**
 * @brief A module of an application requires a module that is not listed before
 * it.
 */
class MissingRequiredModuleException : public EngineException {
 public:
  MissingRequiredModuleException(const std::string& moduleName,
                                 const std::string& requiredModule);
};

/**
 * @brief A module of an application threw while it started.
 */
class ModuleStartFailedException : public EngineException {
 public:
  ModuleStartFailedException(const std::string& moduleName,
                             const std::string& reason);
};
```

`EngineException.cpp`, added before the closing `}  // namespace rtype::engine` (the rest of the file is unchanged):

```cpp
NullModuleException::NullModuleException()
    : EngineException("A null module cannot be given to an application") {}

MissingRequiredModuleException::MissingRequiredModuleException(
    const std::string& moduleName, const std::string& requiredModule)
    : EngineException("The module \"" + moduleName +
                      "\" requires the module \"" + requiredModule +
                      "\", which is not listed before it") {}

ModuleStartFailedException::ModuleStartFailedException(
    const std::string& moduleName, const std::string& reason)
    : EngineException("The module \"" + moduleName +
                      "\" failed to start: " + reason) {}
```

**Reading the code, symbol by symbol.**

- `EngineException`: the root of the engine's errors, `src/engine/Exceptions/EngineException.hpp:11`, a `std::runtime_error` built from a message (`:13`).
- The parameters are named `moduleName`, not `module`: since C++20, a line that starts with `module` followed by a name, `:` or `;` is read as a module declaration, so the code keeps the word out of its identifiers (`listedModule` in `Application.cpp` too).
- The two-parameter constructors are not `explicit`, like `InvalidLaunchOptionValueException` (`EngineException.hpp:109`); the one-parameter rule of `explicit` does not apply.

### Step 3 — The application: `src/engine/Modules/Application/Application.{hpp,cpp}`

**Why.** Someone has to call the modules in a known order, refuse a list that cannot work before anything opens, and undo a half-started program.

**What we do.** Add `Application`: it takes the list, checks it, installs and starts the modules in list order, and stops the started ones in reverse order.

**What it does.**

| Call | State before | Effect |
|---|---|---|
| constructor | a list | throws `NullModuleException` if an entry is null; throws `MissingRequiredModuleException` if a required name is not among the modules listed before; otherwise keeps the list. No module method other than `name()`/`requiredModules()` runs |
| `start()` | never started | `install()` on each module in list order, then `start()` on each in list order; each successful start raises the started count |
| `start()` | a module throws from `start()` | the started modules are stopped, last first; `ModuleStartFailedException(name, what())` is thrown |
| `start()` | already called | nothing |
| `stop()` | n modules started | `stop()` on modules n-1 … 0; the started count goes back to 0 |
| `stop()` | none started | nothing |
| destructor | any | `stop()` |

**How it is used.** In a program (illustration: no program builds an `Application` in this change; the tests in Step 5 do):

```cpp
std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
modules.push_back(std::make_unique<LogModule>());
modules.push_back(std::make_unique<AudioModule>());
rtype::engine::Application application(std::move(modules));
application.start();
runUntilClosed();
application.stop();
```

```cpp
#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include "IModule.hpp"

namespace rtype::engine {

/**
 * @brief Installs, starts and stops the modules of a program in a known order.
 *
 * The order is the one of the list given to the constructor. start() installs
 * every module in that order, then starts every module in that order; stop()
 * stops the started modules in the reverse order. A module comes after every
 * module it requires, so what it requires is installed and started before it,
 * and stopped after it.
 */
class Application {
 public:
  /**
   * @brief Takes the modules and checks the list, before any module is
   * installed.
   *
   * @param modules The modules of the program, in the order they install and
   * start.
   * @throws NullModuleException if a module is null.
   * @throws MissingRequiredModuleException if a module requires a module that
   * is not listed before it.
   */
  explicit Application(std::vector<std::unique_ptr<IModule>> modules);

  /** @brief Stops the modules still started, as stop() does. */
  ~Application();

  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;
  Application(Application&&) = delete;
  Application& operator=(Application&&) = delete;

  /**
   * @brief Installs every module, then starts every module, in list order.
   *
   * Only the first call does anything: a later call, even after stop(),
   * installs and starts nothing. When a module throws from start(), the modules
   * started before it are stopped, the last started first, then the error is
   * thrown; the module that threw is not stopped. An exception thrown by
   * install() goes through unchanged, and no module is started.
   *
   * @throws ModuleStartFailedException if a module throws from start(); its
   * message names the module and gives the reason.
   */
  void start();

  /**
   * @brief Stops the started modules, the last started first.
   *
   * A module is stopped once: a second call stops nothing.
   */
  void stop();

 private:
  void installModules();
  void startModules();

  std::vector<std::unique_ptr<IModule>> modules_;
  std::size_t startedCount_{0};
  bool startCalled_{false};
};

}  // namespace rtype::engine
```

```cpp
#include "Application.hpp"
#include <algorithm>
#include <cstddef>
#include <exception>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "EngineException.hpp"
#include "IModule.hpp"

namespace rtype::engine {

namespace {

void requireNoNullModule(std::span<const std::unique_ptr<IModule>> modules) {
  for (const std::unique_ptr<IModule>& listedModule : modules) {
    if (listedModule == nullptr) {
      throw NullModuleException();
    }
  }
}

bool listsModule(std::span<const std::unique_ptr<IModule>> modules,
                 const std::string& name) {
  return std::ranges::any_of(
      modules, [&name](const std::unique_ptr<IModule>& listedModule) {
        return listedModule->name() == name;
      });
}

void requireModulesListedBefore(
    std::span<const std::unique_ptr<IModule>> modules) {
  for (std::size_t position = 0; position < modules.size(); ++position) {
    const std::span<const std::unique_ptr<IModule>> listedBefore =
        modules.first(position);
    for (const std::string& required : modules[position]->requiredModules()) {
      if (!listsModule(listedBefore, required)) {
        throw MissingRequiredModuleException(modules[position]->name(),
                                             required);
      }
    }
  }
}

}  // namespace

Application::Application(std::vector<std::unique_ptr<IModule>> modules)
    : modules_(std::move(modules)) {
  requireNoNullModule(modules_);
  requireModulesListedBefore(modules_);
}

Application::~Application() { stop(); }

void Application::start() {
  if (startCalled_) {
    return;
  }
  startCalled_ = true;
  installModules();
  startModules();
}

void Application::stop() {
  while (startedCount_ > 0) {
    --startedCount_;
    modules_[startedCount_]->stop();
  }
}

void Application::installModules() {
  for (const std::unique_ptr<IModule>& listedModule : modules_) {
    listedModule->install();
  }
}

void Application::startModules() {
  for (const std::unique_ptr<IModule>& listedModule : modules_) {
    try {
      listedModule->start();
    } catch (const std::exception& error) {
      stop();
      throw ModuleStartFailedException(listedModule->name(), error.what());
    }
    ++startedCount_;
  }
}

}  // namespace rtype::engine
```

**Reading the code, symbol by symbol.**

- `requireNoNullModule(modules)`: returns nothing; throws `NullModuleException` at the first null entry. Takes the list as a `std::span` of `const std::unique_ptr<IModule>`, built implicitly from `modules_`.
- `listsModule(modules, name)`: `true` if one of `modules` has `name()` equal to `name`, `false` otherwise (`false` for an empty span).

  | `modules` holds a module named `name` | result |
  |---|---|
  | yes | `true` |
  | no, or `modules` empty | `false` |

- `requireModulesListedBefore(modules)`: for each position, `listedBefore` is the modules before it (`modules.first(position)`, empty for the first module); throws `MissingRequiredModuleException(modules[position]->name(), required)` for the first required name `listsModule` does not find there. So the first module may require nothing.
- These three free functions live in the anonymous namespace of `Application.cpp`: no declaration of the header names them (`fix-architecture-no-type-nested-in-a-class.md` point 3), as `findLaunchOption` does in `LaunchArguments.cpp:14`. The checks are separate from the writes (`fix-architecture-a-function-does-one-kind-of-step.md`): the constructor only moves the list in, then calls the two checks.
- `modules_`: the list, in the program's order. `startedCount_`: how many modules, from the front of the list, have returned from `start()`; the started modules are always `modules_[0 .. startedCount_ - 1]`. `startCalled_`: whether `start()` already ran (D5). The built-in members have default member initializers, as `fix-execution-run-clang-tidy-before-push.md` requires.
- `stop()` lowers `startedCount_` before each call, so a module is stopped once even if `stop()` is called again; with `startedCount_` at 0 it does nothing.
- `startModules()` catches `const std::exception&` only: `IModule::start()` documents that a module reports a failure that way. `stop()` inside the `catch` stops the modules before the one that threw, since `startedCount_` was not raised for it.
- Copy and move are deleted: the destructor stops modules, so a copy would stop them twice; `cppcoreguidelines-special-member-functions` asks for the five once the destructor is declared, as `NetworkContext.hpp:21-26` does.
- Member functions: `start`, `stop`, `installModules`, `startModules`: 4, under the 10 of `fix-architecture-at-most-ten-methods-per-class.md`. Parameters: at most 2 everywhere (`fix-architecture-at-most-three-parameters.md`).

### Step 4 — The build: CMake

**Why.** Each folder declares its own target, and `rtype_engine` exposes everything the engine offers.

**What we do.** Two new `CMakeLists.txt`, modeled on `src/engine/Systems/CMakeLists.txt` and `src/engine/Systems/SystemScheduler/CMakeLists.txt`, and the engine's list extended.

**What it does.** `rtype_engine_modules` (INTERFACE) exposes `IModule.hpp`; `rtype_engine_application` (STATIC) builds `Application.cpp` with the project's warnings; both reach every user of `rtype_engine`.

**How it is used.** A program or a test links `rtype_engine`, as `engine_tests` does (`tests/CMakeLists.txt:43`), and includes `"Application.hpp"` and `"IModule.hpp"`.

`src/engine/Modules/CMakeLists.txt` (new):

```cmake
add_library(rtype_engine_modules INTERFACE)

target_include_directories(rtype_engine_modules INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Application)
```

`src/engine/Modules/Application/CMakeLists.txt` (new):

```cmake
add_library(rtype_engine_application STATIC Application.cpp)

target_include_directories(rtype_engine_application PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_application
  PUBLIC
    rtype_engine_modules
    rtype_engine_exceptions
)

rtype_enable_warnings(rtype_engine_application)
```

`src/engine/CMakeLists.txt`: one `add_subdirectory` after `LaunchArguments`, two targets at the end of the list (the rest unchanged):

```cmake
add_subdirectory(LaunchArguments)
add_subdirectory(Modules)
```

```cmake
    rtype_engine_launch_arguments
    rtype_engine_modules
    rtype_engine_application
)
```

**Reading the code, symbol by symbol.**

- `rtype_enable_warnings()`: the root `CMakeLists.txt:16`; `-Wall -Wextra -Wpedantic`, `/W4 /permissive-` on MSVC.
- `rtype_engine_exceptions`: `src/engine/Exceptions/CMakeLists.txt`, linked `PUBLIC` so a caller of `Application` can catch its exceptions, as `rtype_engine_system_scheduler` does.
- `rtype_engine_modules` links nothing: `IModule.hpp` includes only `<string>` and `<vector>`.

### Step 5 — The tests

**Why.** The order, the rollback and the refusal are the whole contract of the feature; each one needs a test that fails when it breaks (measured: see Test strategy).

**What we do.** A module double that writes its calls in a shared journal, fixtures that build lists of such modules, constants written from the requirements, and the 15 tests.

**What it does.** Each test builds an `Application` from recording modules, acts once, and compares the journal (or the error) with the expected value.

**How it is used.** `engine_tests` runs them: `ctest --preset test -R '^Application\.'`.

`tests/doubles/ModuleJournal.hpp` (new):

```cpp
#pragma once

#include <string>
#include <vector>

/** @brief The calls the recording modules of one test received, in order,
 * shared by those modules: each appends a line such as "install first". */
struct ModuleJournal {
  std::vector<std::string> calls;
};
```

`tests/doubles/RecordingModuleSetup.hpp` (new):

```cpp
#pragma once

#include <string>
#include <vector>

/** @brief What a RecordingModule answers and how it behaves. */
struct RecordingModuleSetup {
  std::string name;
  std::vector<std::string> requiredModules;
  /** @brief Whether start() throws a StartRefusedException after writing its
   * call down. */
  bool failsToStart{false};
};
```

`tests/doubles/RecordingModuleConstants.hpp` (new):

```cpp
#pragma once

/** @brief What a RecordingModule writes in its journal before its name, for
 * each call. */
constexpr const char* INSTALL_CALL = "install ";
constexpr const char* START_CALL = "start ";
constexpr const char* STOP_CALL = "stop ";

/** @brief The message of the StartRefusedException a RecordingModule throws. */
constexpr const char* START_REFUSAL_REASON = "the test refuses this start";
```

`tests/doubles/StartRefusedException.hpp` and `.cpp` (new):

```cpp
#pragma once

#include <stdexcept>

/** @brief What a RecordingModule set to fail throws from start(); it stands for
 * any error a real module may throw. */
class StartRefusedException : public std::runtime_error {
 public:
  StartRefusedException();
};
```

```cpp
#include "StartRefusedException.hpp"
#include <stdexcept>
#include "RecordingModuleConstants.hpp"

StartRefusedException::StartRefusedException()
    : std::runtime_error(START_REFUSAL_REASON) {}
```

`tests/doubles/RecordingModule.hpp` and `.cpp` (new):

```cpp
#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "IModule.hpp"
#include "ModuleJournal.hpp"
#include "RecordingModuleSetup.hpp"

/** @brief Module double that writes each call it receives in a journal shared
 * with the other modules of the test; it opens nothing. */
class RecordingModule : public rtype::engine::IModule {
 public:
  /** @param journal Where the calls are written down; it must outlive the
   * module. */
  RecordingModule(RecordingModuleSetup setup, ModuleJournal& journal);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> requiredModules() const override;
  void install() override;
  /** @throws StartRefusedException after writing the call down, when the setup
   * says the module fails to start. */
  void start() override;
  void stop() override;

 private:
  void record(std::string_view call);

  RecordingModuleSetup setup_;
  ModuleJournal* journal_;
};
```

```cpp
#include "RecordingModule.hpp"
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ModuleJournal.hpp"
#include "RecordingModuleConstants.hpp"
#include "RecordingModuleSetup.hpp"
#include "StartRefusedException.hpp"

RecordingModule::RecordingModule(RecordingModuleSetup setup,
                                 ModuleJournal& journal)
    : setup_(std::move(setup)), journal_(&journal) {}

std::string RecordingModule::name() const { return setup_.name; }

std::vector<std::string> RecordingModule::requiredModules() const {
  return setup_.requiredModules;
}

void RecordingModule::install() { record(INSTALL_CALL); }

void RecordingModule::start() {
  record(START_CALL);
  if (setup_.failsToStart) {
    throw StartRefusedException();
  }
}

void RecordingModule::stop() { record(STOP_CALL); }

void RecordingModule::record(std::string_view call) {
  journal_->calls.push_back(std::string(call) + setup_.name);
}
```

`tests/engine/Modules/ApplicationTestConstants.hpp` (new):

```cpp
#pragma once

/** @brief The names of the modules of the tests, listed in this order. */
constexpr const char* FIRST_MODULE = "first";
constexpr const char* SECOND_MODULE = "second";
constexpr const char* THIRD_MODULE = "third";

/** @brief A name no module of the tests has. */
constexpr const char* UNLISTED_MODULE = "unlisted";

/** @brief The journal lines of the calls the modules of the tests receive. */
constexpr const char* INSTALL_FIRST = "install first";
constexpr const char* INSTALL_SECOND = "install second";
constexpr const char* INSTALL_THIRD = "install third";
constexpr const char* START_FIRST = "start first";
constexpr const char* START_SECOND = "start second";
constexpr const char* START_THIRD = "start third";
constexpr const char* STOP_FIRST = "stop first";
constexpr const char* STOP_SECOND = "stop second";
constexpr const char* STOP_THIRD = "stop third";

/** @brief The error of SECOND_MODULE requiring UNLISTED_MODULE. */
constexpr const char* SECOND_REQUIRES_UNLISTED_MESSAGE =
    "The module \"second\" requires the module \"unlisted\", which is not "
    "listed before it";

/** @brief The error of THIRD_MODULE throwing START_REFUSAL_REASON from start().
 */
constexpr const char* THIRD_FAILED_TO_START_MESSAGE =
    "The module \"third\" failed to start: the test refuses this start";
```

`tests/engine/Modules/ApplicationFixtures.hpp` and `.cpp` (new):

```cpp
#pragma once

#include <memory>
#include <vector>
#include "IModule.hpp"
#include "ModuleJournal.hpp"
#include "RecordingModuleSetup.hpp"

/**
 * @returns One RecordingModule per setup, in the same order, all writing in @p
 * journal, which must outlive them.
 */
std::vector<std::unique_ptr<rtype::engine::IModule>> recordingModules(
    const std::vector<RecordingModuleSetup>& setups, ModuleJournal& journal);

/** @returns FIRST_MODULE, SECOND_MODULE and THIRD_MODULE, which require
 * nothing. */
std::vector<std::unique_ptr<rtype::engine::IModule>> threeModules(
    ModuleJournal& journal);

/** @returns FIRST_MODULE, SECOND_MODULE and THIRD_MODULE, which require
 * nothing; THIRD_MODULE throws from start(). */
std::vector<std::unique_ptr<rtype::engine::IModule>> thirdFailingToStart(
    ModuleJournal& journal);

/** @returns FIRST_MODULE, then SECOND_MODULE, which requires UNLISTED_MODULE.
 */
std::vector<std::unique_ptr<rtype::engine::IModule>> secondRequiringUnlisted(
    ModuleJournal& journal);

/** @returns FIRST_MODULE, which requires SECOND_MODULE, then SECOND_MODULE. */
std::vector<std::unique_ptr<rtype::engine::IModule>> firstRequiringSecond(
    ModuleJournal& journal);

/** @returns FIRST_MODULE, then SECOND_MODULE, which requires FIRST_MODULE. */
std::vector<std::unique_ptr<rtype::engine::IModule>> secondRequiringFirst(
    ModuleJournal& journal);
```

```cpp
#include "ApplicationFixtures.hpp"
#include <memory>
#include <vector>
#include "ApplicationTestConstants.hpp"
#include "IModule.hpp"
#include "ModuleJournal.hpp"
#include "RecordingModule.hpp"
#include "RecordingModuleSetup.hpp"

using rtype::engine::IModule;

std::vector<std::unique_ptr<IModule>> recordingModules(
    const std::vector<RecordingModuleSetup>& setups, ModuleJournal& journal) {
  std::vector<std::unique_ptr<IModule>> modules;
  modules.reserve(setups.size());
  for (const RecordingModuleSetup& setup : setups) {
    modules.push_back(std::make_unique<RecordingModule>(setup, journal));
  }
  return modules;
}

std::vector<std::unique_ptr<IModule>> threeModules(ModuleJournal& journal) {
  return recordingModules(
      {RecordingModuleSetup{.name = FIRST_MODULE, .requiredModules = {}},
       RecordingModuleSetup{.name = SECOND_MODULE, .requiredModules = {}},
       RecordingModuleSetup{.name = THIRD_MODULE, .requiredModules = {}}},
      journal);
}

std::vector<std::unique_ptr<IModule>> thirdFailingToStart(
    ModuleJournal& journal) {
  return recordingModules(
      {RecordingModuleSetup{.name = FIRST_MODULE, .requiredModules = {}},
       RecordingModuleSetup{.name = SECOND_MODULE, .requiredModules = {}},
       RecordingModuleSetup{
           .name = THIRD_MODULE, .requiredModules = {}, .failsToStart = true}},
      journal);
}

std::vector<std::unique_ptr<IModule>> secondRequiringUnlisted(
    ModuleJournal& journal) {
  return recordingModules(
      {RecordingModuleSetup{.name = FIRST_MODULE, .requiredModules = {}},
       RecordingModuleSetup{.name = SECOND_MODULE,
                            .requiredModules = {UNLISTED_MODULE}}},
      journal);
}

std::vector<std::unique_ptr<IModule>> firstRequiringSecond(
    ModuleJournal& journal) {
  return recordingModules(
      {RecordingModuleSetup{.name = FIRST_MODULE,
                            .requiredModules = {SECOND_MODULE}},
       RecordingModuleSetup{.name = SECOND_MODULE, .requiredModules = {}}},
      journal);
}

std::vector<std::unique_ptr<IModule>> secondRequiringFirst(
    ModuleJournal& journal) {
  return recordingModules(
      {RecordingModuleSetup{.name = FIRST_MODULE, .requiredModules = {}},
       RecordingModuleSetup{.name = SECOND_MODULE,
                            .requiredModules = {FIRST_MODULE}}},
      journal);
}
```

`tests/engine/Modules/ApplicationTest.cpp` (new):

```cpp
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ApplicationFixtures.hpp"
#include "ApplicationTestConstants.hpp"
#include "EngineException.hpp"
#include "IModule.hpp"
#include "ModuleJournal.hpp"

using rtype::engine::Application;
using rtype::engine::IModule;
using rtype::engine::MissingRequiredModuleException;
using rtype::engine::ModuleStartFailedException;
using rtype::engine::NullModuleException;

/**
 * Given an application of three modules listed first, second, third
 * When it starts
 * Then it installs the three in list order, then starts the three in list order
 */
TEST(Application, InstallsEveryModuleThenStartsEveryModuleInListOrder) {
  ModuleJournal journal;
  Application application(threeModules(journal));

  application.start();

  EXPECT_EQ(journal.calls, (std::vector<std::string>{
                               INSTALL_FIRST, INSTALL_SECOND, INSTALL_THIRD,
                               START_FIRST, START_SECOND, START_THIRD}));
}

/**
 * Given a started application of three modules listed first, second, third
 * When it stops
 * Then it stops third, then second, then first
 */
TEST(Application, StopsModulesInReverseListOrder) {
  ModuleJournal journal;
  Application application(threeModules(journal));
  application.start();
  journal.calls.clear();

  application.stop();

  EXPECT_EQ(journal.calls,
            (std::vector<std::string>{STOP_THIRD, STOP_SECOND, STOP_FIRST}));
}

/**
 * Given an application that started and stopped
 * When it stops again
 * Then no module is stopped a second time
 */
TEST(Application, SecondStopStopsNothing) {
  ModuleJournal journal;
  Application application(threeModules(journal));
  application.start();
  application.stop();
  journal.calls.clear();

  application.stop();

  EXPECT_TRUE(journal.calls.empty());
}

/**
 * Given a started application of three modules listed first, second, third
 * When it is destroyed without being stopped
 * Then it stops third, then second, then first
 */
TEST(Application, DestructionStopsStartedModulesInReverseListOrder) {
  ModuleJournal journal;
  {
    Application application(threeModules(journal));
    application.start();
    journal.calls.clear();
  }

  EXPECT_EQ(journal.calls,
            (std::vector<std::string>{STOP_THIRD, STOP_SECOND, STOP_FIRST}));
}

/**
 * Given an application that already started
 * When it starts again
 * Then no module is installed or started a second time
 */
TEST(Application, SecondStartDoesNothing) {
  ModuleJournal journal;
  Application application(threeModules(journal));
  application.start();
  journal.calls.clear();

  application.start();

  EXPECT_TRUE(journal.calls.empty());
}

/**
 * Given an application of no module
 * When it starts, then stops
 * Then nothing throws
 */
TEST(Application, ApplicationWithoutModulesStartsAndStops) {
  ModuleJournal journal;
  Application application(recordingModules({}, journal));

  EXPECT_NO_THROW(application.start());
  EXPECT_NO_THROW(application.stop());
}

/**
 * Given a list where second requires a module no entry is named after
 * When an application is built from it
 * Then a MissingRequiredModuleException is thrown
 */
TEST(Application, RefusesAModuleWhoseRequiredModuleIsMissing) {
  ModuleJournal journal;

  EXPECT_THROW(const Application application(secondRequiringUnlisted(journal)),
               MissingRequiredModuleException);
}

/**
 * Given a list where second requires a module no entry is named after
 * When an application is built from it
 * Then the error names second and the missing module
 */
TEST(Application, MissingRequiredModuleErrorNamesTheMissingModule) {
  ModuleJournal journal;

  try {
    const Application application(secondRequiringUnlisted(journal));
    ADD_FAILURE() << "the application was built";
  } catch (const MissingRequiredModuleException& error) {
    EXPECT_EQ(std::string{error.what()}, SECOND_REQUIRES_UNLISTED_MESSAGE);
  }
}

/**
 * Given a list where second requires a module no entry is named after
 * When an application is built from it
 * Then no module is installed, started or stopped
 */
TEST(Application, MissingRequiredModuleCallsNoModule) {
  ModuleJournal journal;

  EXPECT_THROW(const Application application(secondRequiringUnlisted(journal)),
               MissingRequiredModuleException);

  EXPECT_TRUE(journal.calls.empty());
}

/**
 * Given a list where first requires second, listed after it
 * When an application is built from it
 * Then a MissingRequiredModuleException is thrown
 */
TEST(Application, RefusesARequiredModuleListedAfterTheModuleThatNeedsIt) {
  ModuleJournal journal;

  EXPECT_THROW(const Application application(firstRequiringSecond(journal)),
               MissingRequiredModuleException);
}

/**
 * Given a list where second requires first, listed before it
 * When an application is built from it
 * Then nothing throws
 */
TEST(Application, AcceptsARequiredModuleListedBeforeTheModuleThatNeedsIt) {
  ModuleJournal journal;

  EXPECT_NO_THROW(const Application application(secondRequiringFirst(journal)));
}

/**
 * Given a list holding a null module
 * When an application is built from it
 * Then a NullModuleException is thrown
 */
TEST(Application, RefusesANullModule) {
  std::vector<std::unique_ptr<IModule>> modules;
  modules.push_back(nullptr);

  EXPECT_THROW(const Application application(std::move(modules)),
               NullModuleException);
}

/**
 * Given an application of first, second and third, where third fails to start
 * When it starts
 * Then second, then first are stopped, and third is not stopped
 */
TEST(Application, FailedStartStopsTheStartedModulesInReverseListOrder) {
  ModuleJournal journal;
  Application application(thirdFailingToStart(journal));

  EXPECT_THROW(application.start(), ModuleStartFailedException);

  EXPECT_EQ(journal.calls,
            (std::vector<std::string>{INSTALL_FIRST, INSTALL_SECOND,
                                      INSTALL_THIRD, START_FIRST, START_SECOND,
                                      START_THIRD, STOP_SECOND, STOP_FIRST}));
}

/**
 * Given an application where third fails to start
 * When it starts
 * Then the error names third and gives the reason third threw
 */
TEST(Application, FailedStartErrorNamesTheFailingModule) {
  ModuleJournal journal;
  Application application(thirdFailingToStart(journal));

  try {
    application.start();
    ADD_FAILURE() << "the application started";
  } catch (const ModuleStartFailedException& error) {
    EXPECT_EQ(std::string{error.what()}, THIRD_FAILED_TO_START_MESSAGE);
  }
}

/**
 * Given an application whose start failed at third
 * When it stops
 * Then no module is stopped a second time
 */
TEST(Application, StopAfterAFailedStartStopsNothing) {
  ModuleJournal journal;
  Application application(thirdFailingToStart(journal));
  EXPECT_THROW(application.start(), ModuleStartFailedException);
  journal.calls.clear();

  application.stop();

  EXPECT_TRUE(journal.calls.empty());
}
```

`tests/CMakeLists.txt`, the `engine_tests` lines (lines 43-44 today). `SOURCES` gains four files, and `tests/doubles` becomes an include folder of the target (the rest of the line unchanged):

```cmake
rtype_add_test(NAME engine_tests SOURCES CollisionTest.cpp ComponentQueryTest.cpp ComponentTest.cpp EntityIndexMapTest.cpp EntityRegistryTest.cpp EventBusFixtures.cpp EventBusTest.cpp engine/Time/FixedTimestepTest.cpp engine/Time/TimeConstantsTest.cpp engine/LaunchArguments/LaunchArgumentsTest.cpp engine/Modules/ApplicationTest.cpp engine/Modules/ApplicationFixtures.cpp doubles/RecordingModule.cpp doubles/StartRefusedException.cpp NetworkEntityTableTest.cpp NetworkIdAllocatorTest.cpp SystemSchedulerTest.cpp TimerSchedulerTest.cpp LIBS rtype_engine TIMEOUT ${RTYPE_THREAD_TEST_TIMEOUT_SECONDS})
target_include_directories(engine_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/doubles)
```

**Reading the code, symbol by symbol.**

- `ModuleJournal::calls`: the lines the modules wrote, oldest first; the test owns it and declares it before the application, so it outlives the modules (`RecordingModule` keeps a pointer to it, as `RecordingEntityLayer` keeps `record_`).
- `RecordingModuleSetup`: `name` and `requiredModules` are what the double answers; `failsToStart` (default `false`) makes `start()` throw after writing its line. Every construction lists `.requiredModules` even when empty, because GCC's `-Wmissing-field-initializers` reports a skipped member without a default member initializer (`fix-execution-gcc-warns-when-a-designated-initializer-skips-a-member-without-default.md`); `failsToStart` has one and may be skipped.
- `INSTALL_CALL`, `START_CALL`, `STOP_CALL`: the start of each journal line, the module's name follows. `START_REFUSAL_REASON`: the message of `StartRefusedException`, so `THIRD_FAILED_TO_START_MESSAGE` ends with it.
- `RecordingModule::record(call)`: appends `call + name` to the journal; returns nothing. `start()` records before throwing, so the journal shows that the failing module was asked to start, and that it was not stopped.
- `recordingModules(setups, journal)`: returns one `RecordingModule` per setup, in order; reserves first (`performance-inefficient-vector-operation` reported the loop without it). `threeModules`, `thirdFailingToStart`, `secondRequiringUnlisted`, `firstRequiringSecond`, `secondRequiringFirst`: the lists the tests share, named after what they hold.
- The constants are `constexpr const char*`, like `DrawJournal.hpp`, so a `std::vector<std::string>{INSTALL_FIRST, …}` and a designated `.name = FIRST_MODULE` take them as they are.
- The test file holds only includes, `using` lines and `TEST` blocks (`fix-format-a-test-file-holds-only-tests.md`); `EXPECT_THROW(const Application application(…), …)` declares the object inside the macro, so no temporary is created and dropped (`bugprone-unused-raii`).

### Step 6 — Gates (code commit)

Run from the worktree, each output read before the next (`fix-execution-zsh-pipestatus.md`):

```bash
cmake --workflow --preset test > /tmp/rty193-test.log 2>&1; echo "exit: $?"
grep -E 'warning|error' /tmp/rty193-test.log
grep -E 'tests passed|tests failed' /tmp/rty193-test.log
cmake --build build --target format-check > /tmp/rty193-format.log 2>&1; echo "exit: $?"
```

- The test workflow must recompile every new file (`Building CXX object` lines for `Application.cpp`, `EngineException.cpp`, `RecordingModule.cpp`, `StartRefusedException.cpp`, `ApplicationFixtures.cpp`, `ApplicationTest.cpp`), and the `grep` for warnings must print nothing (`fix-execution-zero-warnings-proven-on-a-recompiling-build.md`). The count of tests grows by 15 from `origin/main`.
- clang-tidy on every changed `.cpp` and `.hpp` as a main file, with the SDK flags, `--header-filter='.*'` and `-DINFINITY=__builtin_inff()`, after counting `Error while processing` and `Error parsing` (`fix-execution-run-clang-tidy-before-push.md`).
- Parameters: `clang-tidy -p build --quiet --config='{Checks: "-*,readability-function-size", CheckOptions: {readability-function-size.ParameterThreshold: 3}}'` on the changed `.cpp` (`fix-architecture-at-most-three-parameters.md`).
- Member functions: the `clang-query` of `fix-architecture-at-most-ten-methods-per-class.md` on `Application` (4) and `RecordingModule` (6).
- The greps of `fix-architecture-no-type-nested-in-a-class.md` point 4, `fix-format-a-constants-header-ends-in-constants.md` point 5, `fix-format-a-test-file-holds-only-tests.md` point 5 and `fix-architecture-a-cpp-hpp-pair-has-its-own-folder.md` point 5 on the changed files: all print nothing (measured empty on the scratch copy).
- `git diff --stat origin/main...HEAD -- src/server src/client src/game src/network` prints nothing (no program changes).

Commit 1, once green (index read in one call, commit in the next: `fix-execution-check-the-index-before-committing.md`):

```text
feat: add an engine application that installs, starts and stops modules in order
```

### Step 7 — Documentation (`document-feature` skill)

**Why.** The issue answers a jury remark that the engine's structure is not explicit enough for its user: the site is where that user looks.

**What we do.** Follow `.claude/skills/document-feature/SKILL.md`, on three pages:

1. `docs/src/content/docs/engine.md`
   - the opening sentence gains what the engine now does: "…, reads the options a program is given at launch, and installs, starts and stops the modules a program is made of";
   - the state table gains a row: `| Modules and application | Available; no program uses it yet | src/engine/Modules/ |`;
   - a new section before "Where to intervene", draft below;
   - "Where to intervene" gains a row: `| Add a part a program can switch on or off | A class implementing IModule in src/engine/Modules/IModule.hpp, listed after the modules it requires in the list given to Application |` (backticks on the symbols and the path).
2. `docs/src/content/docs/architecture.md`: a `## Modules` section after `## Systems`, draft below.
3. `docs/src/content/docs/project-layout.md`
   - the engine tree (lines 62-72) gains `├── Modules/          the application and the modules it installs, starts and stops`, after `LaunchArguments/`;
   - line 74 becomes "`EntityRegistry`, `Components`, `Systems`, `Events`, `Collision`, `Time`, `NetworkIds`, `LaunchArguments`, `Modules` and `Exceptions` exist.";
   - line 211, the list of what `tests/doubles/` holds, gains "a module that writes down when it is installed, started and stopped".

Draft of the `engine.md` section (to be checked against the code by the skill's step 6):

````markdown
## Modules: `Application` and `IModule`

`src/engine/Modules/` (`rtype_engine_modules`, `rtype_engine_application`) gives a program one place to plug each of its parts in. A part is a **module**: a class implementing `IModule`, with a `name()`, the `requiredModules()` it needs, and three steps. `install()` prepares it and opens nothing; `start()` opens what it needs to run (a socket, a thread, a file) and throws a `std::exception` when it cannot; `stop()` closes it and must not throw.

- `Application` takes the list of modules once, when it is built, and checks it before anything is installed. A null module throws `NullModuleException`. A module whose required module is not listed **before** it throws `MissingRequiredModuleException`, whose message names both modules. So a missing module stops the program before any socket, thread or file is opened.
- `start()` installs every module in list order, then starts every module in list order. Only the first call does anything.
- `stop()` stops the started modules in the reverse order, so a module is stopped before the modules it requires. Each module is stopped once, and the destructor of `Application` stops the modules still started.
- When a module throws from `start()`, the modules started before it are stopped in the reverse order, then `start()` throws `ModuleStartFailedException`, whose message names the module and gives the reason. The module that threw is not stopped.

No program builds an `Application` yet: the server and the client still assemble their parts in their `main.cpp`. In this example, `LogModule` and `AudioModule` stand for modules a program would write, and `AudioModule` requires `LogModule`:

```cpp
std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
modules.push_back(std::make_unique<LogModule>());
modules.push_back(std::make_unique<AudioModule>());
rtype::engine::Application application(std::move(modules));
application.start();
runUntilClosed();
application.stop();
```

The reasoning behind the order is in [Architecture](/R-Type/architecture/#modules).
````

Draft of the `architecture.md` section:

```markdown
## Modules

A program is a list of modules, objects implementing `IModule`, handed to an `Application`. The application installs them all, then starts them all, in list order, and stops them in the reverse order, so a module is stopped before the modules it requires. The list is checked when the application is built: a module whose required module is not listed before it is refused before anything is installed, so a missing module never leaves a socket or a thread half open. When a start fails, the modules already started are stopped before the error goes up.

Alternative considered: sorting the modules by their requirements. A module could then be listed anywhere, but the order would be computed instead of read in one place, and two modules that require nothing of each other would have no defined order. As with systems, the list is the one place that says what starts after what.
```

Words searched on every page for what the feature replaces (`fix-process-grep-every-page-for-what-the-feature-replaced.md`): the feature replaces nothing; `git grep -n -i -e 'module' -e 'lifecycle' -e 'startup' -- docs/src/content README.md` is read to make sure no page says the engine has no such structure. Then `npm ci` (if `docs/node_modules` is missing) and `npm run build` in `docs/`: exit 0 and "All internal links are valid".

Commit 2:

```text
docs: document the engine application and its modules
```

### Step 8 — Pull request

Draft pull request (`gh pr create --draft`), in English, with the sections of the repository's pull requests (`## Summary` with `Closes RTY-193`, `## Changes`, `## Type of Change`, `## Testing`, `## Checklist`), the commands of Step 6 and their real output in `## Testing`, and the pre-existing inconsistencies of the Assumptions section reported. No AI mention and no `Co-Authored-By` trailer in the commits or the pull request. Then follow the CI (`fix-process-follow-ci-after-opening-a-pr.md`): the GCC and MSVC jobs and the Linux clang-tidy job are the first to see this code.

## Main commit type

`feat`

## Issue ID

`RTY-193`: the pull request carries `Closes RTY-193`.

## Rules and skills to apply

- `architecture/`: all of it, in particular `architecture-pure-virtual-interfaces.md` (IModule), `architecture-one-folder-per-concrete-class.md` and `fix-architecture-a-cpp-hpp-pair-has-its-own-folder.md` (placement), `architecture-custom-exceptions-per-module.md` (three exceptions in `EngineException`), `fix-architecture-a-function-does-one-kind-of-step.md` (checks apart from writes), `fix-architecture-at-most-three-parameters.md`, `fix-architecture-at-most-ten-methods-per-class.md`, `fix-architecture-no-type-nested-in-a-class.md`.
- `code-style/`: all of it, in particular `code-style-tests-given-when-then.md`, `fix-format-a-test-file-holds-only-tests.md`, `fix-format-a-constants-header-ends-in-constants.md`, `fix-format-designated-initializers-for-aggregates.md`.
- `fix/architecture/fix-architecture-one-cmakelists-per-folder.md`, `fix-architecture-a-link-never-points-back-at-a-folder-that-links-it.md`.
- `fix/process/fix-process-green-test-locking-a-defect-and-overly-deterministic-fake.md`, `fix-reasoning-a-sequence-asserts-its-starting-state.md`, `fix-reasoning-bounded-contract-check-the-empty-state.md`.
- Gates: `fix-execution-zero-warnings-proven-on-a-recompiling-build.md`, `fix-execution-run-clang-tidy-before-push.md`, `fix-execution-gcc-warns-when-a-designated-initializer-skips-a-member-without-default.md`, `fix-execution-zsh-pipestatus.md`, `fix-execution-check-the-index-before-committing.md`, `fix-execution-every-build-runs-in-parallel.md`.
- Commits and pull request: `core/commit.md`, `fix-process-no-co-author-trailer.md`, `fix-format-pull-requests-in-english.md`, `fix-format-pr-links-its-linear-issue.md`, `fix-process-pr-always-opened-as-draft.md`, `fix-process-follow-ci-after-opening-a-pr.md`.
- Skill: `document-feature` (Step 7), with `fix-process-grep-every-page-for-what-the-feature-replaced.md`.
- Not concerned: the network rules (no packet, no socket in this change).

## On-screen check for the user

Nothing changes on screen or in the logs: no program builds an `Application` yet. The check is that both programs behave as before, and that the new tests run.

| Action | Expected on screen | Expected in the logs |
|---|---|---|
| `cmake --workflow --preset test` | the summary lists 15 more tests than on `origin/main`, all passing | no `warning` line in the build output |
| `build/tests/engine_tests --gtest_filter='Application.*'` | `[  PASSED  ] 15 tests.` | none |
| `cmake --workflow --preset build`, then `./r-type_server` and `./r-type_client` | the same title screen, lobby and game as on `origin/main` | the same lines as on `origin/main` |

## Assumptions

- **The design document, read from the user's copy.** The issue links a claude.ai artifact, which the Docs connector still cannot open in this session. The user then attached a Markdown export: `~/Downloads/Système de modules R-Type (Copy).md`, the same name as in the task's attachment record, 542 lines, "R-Type · architecture proposée · 26 octobre 2026". The sections used are "Décisions", "Les mots de cette page", "La proposition" 1 to 5, "Ce qu'on ne fait pas" and "Plan par étapes". The plan assumes this export matches the linked artifact.
- **What RTY-194 holds.** The document's step 1 lists "`Application`, l'interface de module, le contexte d'installation, les services, les étapes, les exceptions du module, les tests", and its "Dans Linear" list maps step 1 to RTY-193 and RTY-194. RTY-193's text names only the application, the interface and the lifecycle. The plan therefore takes the installation context, the services and the stages to be RTY-194. This could not be checked: the plan step does not read Linear. If RTY-193 is meant to carry them too, the plan must grow before it runs.
- **"Le système de plugin".** The user asked to design "the plugin system" from this document. The document makes plugins a separate step (step 6, RTY-205 and RTY-208, cycle 6, after the modules exist), and RTY-193 says nothing of plugins. The plan therefore designs the module interface so that a plugin can implement it unchanged (D10), and builds no loader. Building the loader here would mean taking RTY-205 into this pull request: that is a choice for the user.
- **Branch.** The rules want a branch `<type>/<slug>` started from `origin/main` (`fix-process-branch-before-first-commit.md`); this run works on the branch the harness created, `cez/0a1f079c`, which sits at `origin/main` (`4db8e52`, checked with `git rev-parse HEAD origin/main`). Renaming it, if wanted (for example `feat/engine-application-modules`), is the user's call; the pull request should say which branch carries it.
- **Attribution.** The harness asks for a `Co-Authored-By` trailer and a "Generated with" line; `fix-process-no-co-author-trailer.md` forbids both and states it takes precedence. The plan follows the repository rule: no trailer, no AI mention.
- **Parameter rule.** `fix-architecture-at-most-three-parameters.md` says `.clang-tidy` does not check the limit; it is measured with the command of its point 6.
- **CI-only differences.** `misc-include-cleaner` reads libstdc++ on the CI and libc++ here; the includes of each file were matched against the symbols it names, but only the CI's clang-tidy job settles it. GCC (`-Wmissing-field-initializers`) and MSVC (`/W4`) were not run on this code.
- **Pre-existing, reported, not changed.** The older engine tests sit at the root of `tests/` (`SystemSchedulerTest.cpp`, …) and `tests/engine/LaunchArguments/LaunchArgumentsTest.cpp` holds an anonymous namespace with a struct and a class (before `fix-format-a-test-file-holds-only-tests.md`); this change does not touch those files.
- **`.context/context.md`** does not exist in this worktree (it is local and ignored by git); `.cezar-work/context.md` was the exploration's record, and its "Écarts issue / code" are carried into this section and into D1 to D10.

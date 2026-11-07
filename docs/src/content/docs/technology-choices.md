---
title: Technology choices
description: Why the project uses SFML, Asio, vcpkg, CMake and, for its web site, Next.js, and what each was compared with.
---

The project rests on four tools: SFML draws the client, Asio carries the datagrams, vcpkg installs both and CMake builds everything. The web site, a program of its own that calls the master, has tools of its own, Next.js and bun, which CMake never builds (see [Web site](#web-site-nextjs)). This page states the criteria the team used, compares each tool with its main alternatives, and says what each choice costs. The decisions about the code itself (libraries, ECS, network model) are in [Architecture](/R-Type/architecture/).

## Criteria

Every comparison below uses the same four criteria, in this order.

| Criterion | Question |
|---|---|
| Cross-platform | Does it build and run on Linux (GCC), Windows (MSVC) and macOS, the three systems the team works on? |
| Scope | Does it cover what R-Type needs, and little more? A library that does less than needed adds a second one; one that does more adds code nobody reads. |
| License | May the project, a school deliverable that is also public, link it without condition on its own license? |
| Learning curve | Can a teammate who has never used it write working code from its documentation in a day? |

The first two criteria decide most cases; the license and the learning curve separate the tools that remain.

## Graphics, window and audio: SFML

The client needs a window, keyboard and pointer events, 2D drawing, text and sound. It needs no 3D and no network code of its own. The pinned version is SFML 3.0.2, with only the `window`, `graphics` and `audio` features enabled in `vcpkg.json`.

| | SFML | SDL | Raylib |
|---|---|---|---|
| Cross-platform | Linux, Windows, macOS | Linux, Windows, macOS | Linux, Windows, macOS |
| Scope | window, 2D graphics, audio, network | window, input, audio, rendering | window, 2D and 3D graphics, audio |
| License | zlib | zlib | zlib/libpng |
| Language | C++, objects own their resources | C, functions and handles | C, functions and structs |

All three are cross-platform and permissive, so the language decides. SFML is written in C++: a `sf::Texture` or a `sf::RenderWindow` frees itself when it goes out of scope, and the code of the client calls them like any other class. SDL and Raylib are C libraries: the same code would wrap every handle in a class to get the same guarantee. Raylib is the quickest to start with, but much of what it offers (3D shapes, models) goes unused, and its structures would sit beside the components of the game's own engine (see [Engine](/R-Type/engine/)).

The cost is that SFML stays at a higher level than SDL, which gives finer control of the window and the input devices, and that its `network` module goes unused: it is not enabled, because the server has to run without SFML and Asio covers the need (see below). `r-type_server` never links SFML; the root `CMakeLists.txt` checks it at configure time.

Alternative considered: SDL. It is the most widespread of the three and gives finer control of the window and the input devices. It loses on the learning curve for a team that writes C++20: every resource would need an owning wrapper, written and tested by the team, to match what SFML provides.

## Network I/O: Asio

The server and the client send UDP datagrams, and the server must serve several clients and several games at the same time without one thread per client (see [Network](/R-Type/network/) and [Concurrency](/R-Type/concurrency/)).

| | Asio | Native sockets |
|---|---|---|
| Cross-platform | one API for Linux, Windows and macOS | Berkeley sockets on Linux and macOS, Winsock on Windows |
| Scope | sockets, timers, asynchronous operations, one event loop | sockets only; the event loop (`poll`, `epoll`, `IOCP`) is written by the team |
| License | Boost Software License 1.0 | system API |
| Learning curve | the asynchronous style (handlers) must be learned first | the calls are simple; the portable loop around them is not |

The two differ in what is left to write. With native sockets, the code for Windows and the code for Linux and macOS differ in headers, types, error codes and start-up (`WSAStartup`), and the event loop that waits on many sockets and timers is a different mechanism on each system. Asio provides one loop and one set of types: `rtype_network` owns it in `NetworkContext/` and the UDP socket in `Transport/UdpSocket/`, and it is the only library that includes Asio.

The project uses standalone Asio 1.32.0, not Boost.Asio. It is header-only, so it adds no library to link, and it avoids pulling in Boost for one component. The cost is the asynchronous style: a handler runs later, on a thread the caller does not choose, and a buffer given to an operation must stay alive until its handler has run.

Alternative considered: native sockets with a blocking `recvfrom` in one thread per socket. It is the shortest code for one client. It loses once the server runs several games and timers: each blocked thread needs its own stop mechanism, and the Windows and POSIX differences are written twice.

## Dependencies: vcpkg

The project depends on SFML, Asio, toml++ and, for tests, GoogleTest. A teammate must obtain the same versions on three systems with one command.

| | vcpkg | Conan | CPM |
|---|---|---|---|
| Cross-platform | Linux, Windows, macOS | Linux, Windows, macOS | Linux, Windows, macOS |
| Scope | a C++ package manager with a curated registry | a C++ package manager with a recipe per package and a public registry | a CMake script on top of `FetchContent`; it downloads sources and has no registry of its own |
| License | MIT | MIT | MIT |
| Learning curve | one `vcpkg.json` and one toolchain file | a `conanfile`, a profile and a Python installation | one `CPMAddPackage` line per dependency |

The three licenses do not separate them. The scope does. CPM builds each dependency from source when CMake configures, so every dependency that needs system libraries or its own build options (SFML needs X11 and OpenGL development packages on Linux) is configured by the project itself. Conan and vcpkg both have a recipe for SFML that already does it.

Between those two, the project took vcpkg:

- the dependencies are one file, `vcpkg.json`, with features (`tests` adds GoogleTest) and a `builtin-baseline` that pins every version to one commit of the registry;
- vcpkg itself is a pinned git submodule (`vcpkg/`), and the CMake presets point to its toolchain file, so `cmake --workflow --preset build` is the whole procedure: nothing to install before it except the system packages of the README;
- no other language runtime is needed, where Conan runs on Python;
- the CI keeps the built packages in a binary cache keyed by the content of `vcpkg.json`, so a run that changes no dependency skips their build.

The cost is that the first build compiles SFML and its dependencies from source, which takes minutes on a clean machine, and that a version outside the registry at the pinned baseline needs a change of baseline for all dependencies at once.

Alternative considered: Conan. Its binary packages and its profiles give finer control of the compiler settings, and it handles several versions of a library side by side. It loses on the setup for this project: every teammate would install Python and Conan, write a profile per system and keep the `conanfile` and the CMake presets in step, for a project with four dependencies.

## Build: CMake

CMake 4.2 or newer builds the project with C++20 and the presets `default` (Release), `test` (Debug with the tests), `sanitize` (the tests under AddressSanitizer and UndefinedBehaviorSanitizer) and `modules-off` (the tests with every optional module left out). The CI builds with GCC on Linux and MSVC on Windows.

| | CMake | Meson | Bazel |
|---|---|---|---|
| Cross-platform | Linux, Windows, macOS; generates Makefiles, Ninja or Visual Studio projects | Linux, Windows, macOS; builds with Ninja | Linux, Windows, macOS |
| Scope | generates the build for the tool of each system | build system with its own language | build system for large repositories, with its own language and rules |
| License | BSD 3-clause | Apache 2.0 | Apache 2.0 |
| Learning curve | the language is awkward, but the most documented | a cleaner language, new to the team | the longest to learn |

CMake is the common denominator. SFML, Asio and GoogleTest are found through it (`find_package`) once vcpkg has installed them, and the vcpkg toolchain file is a CMake file. The rest of the choice follows from the dependency: a different build system would need another way to find the libraries.

The cost is the language. A mistake in a `CMakeLists.txt` often shows up as a missing symbol at link time, not as an error at the line. The project limits this with one `CMakeLists.txt` per folder, one library per module and explicit links, with the forbidden ones (`rtype_forbid_links`) checked at configure time.

Alternative considered: Meson. Its language is easier to read and it configures faster. It loses because the team would give up the CMake integration of vcpkg and learn a second tool in the middle of the project.

## Web site: Next.js

The web site shows the game to the team and the players: the download, the servers and their games, hosting a server, the leaderboard (with example scores until the master keeps some) and the wiki; the administration is not written yet. It is a program of its own beside the master, which serves its API only. The site uses the tools of the other web projects of the team: Next.js with the App Router and TypeScript, bun to install and run, Biome to lint and format, Tailwind and shadcn for the look, with class-variance-authority for the variants of a component, and GSAP for the animations, Lenis smoothing the scroll from the ticker of GSAP: a component animates through the `useGSAP()` hook of `@gsap/react`, which undoes its animations when the component goes, and the plugins (`ScrollTrigger` today) are registered once in `web/src/lib/gsap/`, which every component imports from. GSAP is under GreenSock's "no charge" license, which covers its plugins too. Lenis is under MIT, class-variance-authority under Apache 2.0.

| | Next.js server | React with Vite | Astro |
|---|---|---|---|
| Cross-platform | runs anywhere bun and Node.js run | the same | the same |
| Scope | pages and routing, React components rendered on the server, server-only calls to the API | a bundler: routing, the layout of pages and any server are left to the project | static pages first, React only in islands |
| License | MIT | MIT | MIT |
| Learning curve | known by the team, who use it on other projects | known, with a router to choose and wire | known from `docs/`, less for an interactive page |

The Next.js server renders the pages and calls the master itself, so the browser talks to one origin and the master needs no CORS, and what the administration will need (a secret kept on the server, a session in a cookie) is available. The cost is a second program to run in production, with Node.js.

Alternative considered: a static export served by the master. It leaves one program to run, but forbids rendering on the server, Server Actions and cookies, so the secret of the administration would have to live in the browser.

## Where each choice is visible

| Tool | Where | Version |
|---|---|---|
| SFML | `src/client/`, with the `window`, `graphics` and `audio` features of `vcpkg.json` | 3.0.2 |
| Asio | `src/network/` only | 1.32.0 (standalone) |
| vcpkg | `vcpkg.json`, the `vcpkg/` submodule, the toolchain file of `CMakePresets.json` | registry baseline `9e593bb` |
| CMake | the `CMakeLists.txt` of each folder, `CMakePresets.json` | 4.2 or newer |
| Next.js | `web/`, with `web/next.config.ts` | 16.4.5 |
| bun | `web/package.json` (`packageManager`), `web/bun.lock` | 1.3.14 |

A change of one of these tools is a decision that touches every team. It starts as a proposal in [Architecture](/R-Type/architecture/), with the same four criteria.

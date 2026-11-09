---
title: Test and contribute
description: The test presets, the formatting and lint targets, and how to preview the documentation and the web site.
sidebar:
  label: Test and contribute
  order: 4
---

Every pull request runs the same checks as below on Linux (GCC) and Windows (MSVC), with warnings treated as errors. Running them locally first saves a round trip.

## Tests

```bash
cmake --workflow --preset test
```

This builds in Debug with the tests, then runs them. It must pass with zero warnings.

| Preset | Builds in | Adds |
|---|---|---|
| `test` | `build/` | the GoogleTest suites |
| `sanitize` | `build-sanitize/` | AddressSanitizer and UndefinedBehaviorSanitizer; GCC or Clang only |
| `modules-off` | `build-modules-off/` | every [optional module](/R-Type/getting-started/build/#optional-modules) turned off |
| `bench` | `build-bench/` | the tests and tools in Release, for the [time the scripts take](/R-Type/scripting/#what-a-script-costs) |

:::tip[Stress the server]
Once the `test` preset is built, `./build/tests/malformed_datagrams --server 4242` sends a running server malformed datagrams for a minute, and `./build/tests/load_test_bots --bots 32 --seconds 60` has 32 players join and play. See [Network](/R-Type/network/).
:::

## Formatting and lint

| Target | Effect |
|---|---|
| `format` | formats every source file in place |
| `format-check` | fails if a source file is not formatted |
| `tidy` | runs `clang-tidy` on every source file |

```bash
cmake --build build --target format
```

## This documentation

The site is built with [Starlight](https://starlight.astro.build); each page is a Markdown file in `docs/src/content/docs/`. With Node.js 22.12 or newer:

```bash
cd docs
npm ci
npm run dev
```

:::note
`npm run build` fails on a broken internal link, like the `docs` check of pull requests.
:::

## The web site

[rtype.app](https://rtype.app) lives in `web/`: a Next.js server, run with [bun](https://bun.sh) 1.3, that reads the API of the master. With a master running on port 8080:

```bash
cd web
bun install
bun run dev
```

The site is then on `http://localhost:3000`, in French and English.

## Contributing

How to branch, write a commit, open a pull request and know when a change is done is in [the contributing guide](https://github.com/nique-epi/R-Type/blob/main/CONTRIBUTING.md). To add code, start with [Project layout](/R-Type/project-layout/) and [Tutorial: write a module](/R-Type/tutorial-write-a-module/).

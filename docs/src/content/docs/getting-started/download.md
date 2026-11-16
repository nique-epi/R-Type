---
title: Download the game
description: Install the R-Type client from rtype.app on Windows, macOS or Linux, or the server for a host.
sidebar:
  label: Download
  order: 1
---

Players do not need to build anything. Every release of R-Type is published with installers for the three systems, and [rtype.app/download](https://rtype.app/download) picks the right one for your system.

:::tip[The fastest way to play]
Open [rtype.app/download](https://rtype.app/download), press **Download**, install, then press PLAY on the title screen: the server map lists the servers that are online.
:::

## Client

| System | File | Install |
|---|---|---|
| Windows | `R-Type-client-<version>-win64.exe` | Open the installer; R-Type then starts from the Start menu. |
| macOS (Apple Silicon) | `R-Type-client-<version>-Darwin.dmg` | Drag R-Type into Applications. |
| Linux | `R-Type-client-<version>-Linux.AppImage` | Make it executable (`chmod +x`), then run it. |

The client needs a graphics card with OpenGL, a screen of at least 960 × 540, and outgoing UDP open to the server it joins.

:::note[macOS]
A disk image signed and notarized by the CI opens without a warning. A build signed without an identity makes macOS ask once, the first time, to allow it in System Settings > Privacy & Security.
:::

:::caution[Linux]
The AppImage needs FUSE, which most desktops have, and a glibc at least as recent as the one of the machine that built it (Ubuntu 24.04 on the CI).
:::

## Server

A host installs the server alone: `R-Type-server-<version>-win64.exe` or `.zip` on Windows, `R-Type-server-<version>-Linux.tar.gz` and `R-Type-server-<version>-Darwin.tar.gz` elsewhere. Every file of a release is listed on the [GitHub releases page](https://github.com/nique-epi/R-Type/releases), which rtype.app also links to.

Once installed, [Play a game](/R-Type/getting-started/play/) shows how to start a server and join it, and [Configuration commands](/R-Type/commands/) lists every setting of `server.cfg`.

## Building the packages yourself

`cmake --workflow --preset package` writes the same files in `build/`: see [Packages](/R-Type/getting-started/build/#packages).

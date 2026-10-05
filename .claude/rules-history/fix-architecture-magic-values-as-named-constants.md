# History: magic values as named constants

Rule imported from another project when R-Type started: its original incident belongs to another repository.

## Update (2026-10-03) — constants always in their own file
The plan for the client window put `windowWidth`, `windowHeight`, `windowTitle` and the frame limit in an anonymous namespace at the top of `GameWindow.cpp`, as point 2 allowed for file-local values. The user asked for them in a separate file. Point 2 now requires a dedicated constants header named after the concept (`WindowConstants.hpp`), even for a single reader.

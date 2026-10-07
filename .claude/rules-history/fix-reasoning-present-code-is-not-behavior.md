# History: present code is not behavior

The rule was imported from earlier projects; its original incident is not recorded here.

## Update (2026-10-07): "no OpenGL call" read in one class, not in its base

### Context
The client asset tests had to run on the Linux CI, which has no display. The plan relied on corrupt texture files failing to load without creating an OpenGL context.

### Mistake
The claim came from reading `Texture.cpp`: the constructor delegates to the default one and fails in `sf::Image` before any GL call. `sf::Texture` derives from `GlResource`, whose constructor creates the shared context; on Linux without a display SFML aborts with "Failed to open X11 display". The test aborted on the CI, and the pull request description stated the opposite.

### Root cause
Only the method bodies of the class were read; the base class constructor, which runs first, was not. The claim was then written as verified instead of "not verified until it runs on the CI".

### Rule
A claim that code never reaches a resource covers every constructor on the path, base classes included, and is marked not verified until it has run in the target environment.

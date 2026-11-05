#pragma once

/** @brief Whether the module a plugin created was destroyed, kept by the test:
 * the module of a test plugin sets it from its destructor. */
struct PluginLifetime {
  bool moduleDestroyed{false};
};

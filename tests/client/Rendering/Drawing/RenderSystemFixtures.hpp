#pragma once

#include <string>
#include <vector>

/** @returns The images of the seven layers, one per layer, from the
 * background to the interface: the order RenderSystem must draw them in. */
std::vector<std::string> allLayersInOrder();

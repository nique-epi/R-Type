#pragma once

#include "BackgroundDescription.hpp"
#include "BuiltLayers.hpp"
#include "LayerSources.hpp"

namespace rtype::client {

/**
 * @brief Builds what each layer of @p background shows, by the kind of its
 * content, and its emission groups, each with the layers of particles that
 * name it.
 *
 * The LDtk project of a layer of tiles or of props is read here, once. One
 * that cannot be read is told in the journal, and its layer shows nothing:
 * a background a server sent never stops the client.
 */
[[nodiscard]] BuiltLayers layersOf(const BackgroundDescription& background,
                                   const LayerSources& sources);

}  // namespace rtype::client

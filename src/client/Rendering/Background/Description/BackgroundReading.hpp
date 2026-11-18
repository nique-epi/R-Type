#pragma once

#include "BackgroundDescription.hpp"
#include "BackgroundText.hpp"

namespace rtype::client {

/**
 * @brief Reads a background file, as strictly as an entity sheet: every table
 * holds only the keys BackgroundFileConstants.hpp lists for it, each a value
 * of its kind within its range, and every error names the file, the line, the
 * column and the dotted key.
 *
 * The root holds the layers, `[[layer]]`, from the farthest to the nearest,
 * and may hold emission groups, `[[group]]`. A layer holds its name, may hold
 * `motion` and `entrance`, and holds exactly one content: `image`, `tiles`,
 * `particles` or `props`. Particles are a population or belong to a group,
 * and are drawn as a pixel of the palette or with sprites. Nothing is drawn
 * and no file a layer names is opened: see BackgroundCatalog for those.
 *
 * @returns The background @p text describes, named after it.
 * @throws engine::DataFileSyntaxException when the text is not TOML;
 * engine::MissingDataFileKeyException, UnknownDataFileKeyException,
 * DataFileTableExpectedException, DataFileTablesExpectedException,
 * DataFileTextExpectedException, DataFileFlagExpectedException,
 * DataFileValueOutOfRangeException, DataFileDecimalOutOfRangeException,
 * DataFileRangeExpectedException or UnknownDataFileChoiceException for a key
 * that is missing, unknown, of another kind, out of its range or, for a
 * color, not a name of the palette; TooManyBackgroundTablesException for an
 * array longer than a background may hold; and
 * InconsistentBackgroundException for keys that do not go together: a layer
 * with no content or several, a second entrance, particles with both a
 * population and a group or neither, a group no `[[group]]` declares or two
 * of the same name, particles with both a pixel and sprites or neither, a
 * twinkle color without a pixel that twinkles, particles of a group that
 * would never end, and a file that is not named by an asset id.
 */
[[nodiscard]] BackgroundDescription readBackground(const BackgroundText& text);

}  // namespace rtype::client

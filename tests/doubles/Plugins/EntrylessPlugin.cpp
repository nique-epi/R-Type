/** @brief A shared library that exports no rtypePluginEntry: the loader must
 * refuse it by name. It holds one function nothing exports, so the library is
 * not empty. */
namespace {

[[maybe_unused]] int entrylessPluginMarker() { return 0; }

}  // namespace

#include <gtest/gtest.h>
#include <vector>
#include "EngineException.hpp"
#include "ResourceFixtures.hpp"
#include "ResourceManifest.hpp"
#include "ResourceManifestReading.hpp"
#include "ResourceManifestReadingTestConstants.hpp"
#include "TemporaryFolder.hpp"

using rtype::engine::InvalidResourceNameException;
using rtype::engine::MissingDataFileKeyException;
using rtype::engine::readResourceManifest;
using rtype::engine::ResourceDependenciesExpectedException;
using rtype::engine::ResourceManifest;
using rtype::engine::UnknownDataFileKeyException;
using rtype::engine::UnreadableDataFileException;

/**
 * Given a manifest that describes its resource and needs two others
 * When it is read
 * Then the manifest holds the name of the folder, the description and both
 * dependencies in their order
 */
TEST(ResourceManifestReading, ReadsTheDescriptionAndTheDependencies) {
  const TemporaryFolder root;
  root.write(TEST_MANIFEST, FULL_MANIFEST);

  const ResourceManifest manifest =
      readResourceManifest(root.path() / TEST_RESOURCE_NAME);

  EXPECT_EQ(manifest.description, FULL_MANIFEST_DESCRIPTION);
  EXPECT_EQ(manifest.dependencies, textsOf(FULL_MANIFEST_DEPENDENCIES));
}

/**
 * Given a manifest that leaves out its dependencies
 * When it is read
 * Then the resource needs nothing
 */
TEST(ResourceManifestReading, NeedsNothingWhenTheDependenciesAreLeftOut) {
  const TemporaryFolder root;
  root.write(TEST_MANIFEST, MANIFEST_WITHOUT_DEPENDENCIES);

  const ResourceManifest manifest =
      readResourceManifest(root.path() / TEST_RESOURCE_NAME);

  EXPECT_TRUE(manifest.dependencies.empty());
}

/**
 * Given a resource folder without a manifest
 * When it is read
 * Then the read is refused for the file
 */
TEST(ResourceManifestReading, RefusesAFolderWithoutAManifest) {
  const TemporaryFolder root;

  EXPECT_THROW(
      static_cast<void>(readResourceManifest(root.path() / TEST_RESOURCE_NAME)),
      UnreadableDataFileException);
}

/**
 * Given a manifest holding a key a manifest does not have
 * When it is read
 * Then the read is refused for that key
 */
TEST(ResourceManifestReading, RefusesAnUnknownKey) {
  const TemporaryFolder root;
  root.write(TEST_MANIFEST, MANIFEST_WITH_AN_UNKNOWN_KEY);

  EXPECT_THROW(
      static_cast<void>(readResourceManifest(root.path() / TEST_RESOURCE_NAME)),
      UnknownDataFileKeyException);
}

/**
 * Given a manifest without a description
 * When it is read
 * Then the read is refused for the missing key
 */
TEST(ResourceManifestReading, RefusesAManifestWithoutDescription) {
  const TemporaryFolder root;
  root.write(TEST_MANIFEST, MANIFEST_WITHOUT_DESCRIPTION);

  EXPECT_THROW(
      static_cast<void>(readResourceManifest(root.path() / TEST_RESOURCE_NAME)),
      MissingDataFileKeyException);
}

/**
 * Given a manifest whose dependencies are a text instead of a list
 * When it is read
 * Then the read is refused for the dependencies
 */
TEST(ResourceManifestReading, RefusesDependenciesThatAreNotAList) {
  const TemporaryFolder root;
  root.write(TEST_MANIFEST, MANIFEST_WITH_DEPENDENCIES_AS_TEXT);

  EXPECT_THROW(
      static_cast<void>(readResourceManifest(root.path() / TEST_RESOURCE_NAME)),
      ResourceDependenciesExpectedException);
}

/**
 * Given a manifest whose list of dependencies holds a number
 * When it is read
 * Then the read is refused for the dependencies
 */
TEST(ResourceManifestReading, RefusesADependencyThatIsNotAText) {
  const TemporaryFolder root;
  root.write(TEST_MANIFEST, MANIFEST_WITH_A_NUMBER_AS_DEPENDENCY);

  EXPECT_THROW(
      static_cast<void>(readResourceManifest(root.path() / TEST_RESOURCE_NAME)),
      ResourceDependenciesExpectedException);
}

/**
 * Given a manifest that needs a resource named with an uppercase letter
 * When it is read
 * Then the read is refused for the name
 */
TEST(ResourceManifestReading, RefusesADependencyThatIsNotAResourceName) {
  const TemporaryFolder root;
  root.write(TEST_MANIFEST, MANIFEST_WITH_AN_UPPERCASE_DEPENDENCY);

  EXPECT_THROW(
      static_cast<void>(readResourceManifest(root.path() / TEST_RESOURCE_NAME)),
      InvalidResourceNameException);
}

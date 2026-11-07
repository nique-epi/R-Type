---
description: A setting added to an optional module is listed where the tests leave that module out, and the modules-off build is run before the push
trigger: always_on
---

# RULE: A setting added to an optional module is tested with the module off too

Why: `public-address` was added to the master link, an optional module, and to `server.cfg.example`; the default build passed, and the `modules-off` job failed, since `exampleSettingsOfThisBuild()` did not leave the new setting out when the module is not built.

## Rule

1. **Before adding a setting to an optional module** (one with an option `RTYPE_MODULE_<NAME>`), find every place the tests answer for a build without it: `git grep -n '<NAME>_MODULE_BUILT' -- tests`. Each list of the settings or commands of that module there (`ServerRolesFixture::exampleSettingsOfThisBuild()`, a count of refused lines) gains the new one.
2. **Run the tests the change touches in the `modules-off` configuration before the push**: `cmake --preset modules-off -B <scratchpad>/build-modules-off -DVCPKG_INSTALLED_DIR=$PWD/build/vcpkg_installed -DVCPKG_MANIFEST_INSTALL=OFF`, then build the test executables of the change in that folder and run them with `ctest --test-dir`. The default build compiles the module, so it never runs the branches written for its absence.
3. **A test guarded by `if (!<NAME>_MODULE_BUILT) GTEST_SKIP()` is no proof in the default build**: say in the report which configuration ran it.

## Example

- ❌ **Before (wrong)**: `PUBLIC_ADDRESS_SETTING_NAME` added to the master link and `set public-address ""` to `server.cfg.example`; `ServerRolesFixture.AnyOtherSettingOfTheExampleIsTheSupervisors` failed on the `optional modules off` job only.
- ✅ **After (right)**: `exampleSettingsOfThisBuild()` erases `public-address` with `master-url` and `master-key`, and the server module tests pass in a local `modules-off` build before the push.

# RULE: A setting added to an optional module is tested with the module off too

## Context

RTY-241 added the `public-address` setting to `ServerMasterModule`, the master link, which the root `CMakeLists.txt` can leave out with `RTYPE_MODULE_MASTER_LINK=OFF`. The CI runs a `modules-off` job that builds and tests with every optional module off.

## Mistake

The setting went into `SERVER_SETTING_NAMES` and `server.cfg.example`. Every local gate ran in the default build, where the master link is compiled. On the pull request, `Build and test (ubuntu-latest, optional modules off)` failed on `ServerRolesFixture.AnyOtherSettingOfTheExampleIsTheSupervisors`: `exampleSettingsOfThisBuild()` erased `master-url` and `master-key` when the module is not built, but not the new `public-address`, so the test looked for a setting no module of that build declares.

## Root cause

The tests answer for a build without the module in a few places guarded by `MASTER_LINK_MODULE_BUILT`, and none of them is reached by the default build. The change was checked only in the configuration where those branches never run.

## Rule

See the core file of the same name: grep the `<NAME>_MODULE_BUILT` guards of the tests, update their lists, and run the touched tests in a local `modules-off` build before the push.

## Example

- ❌ The new setting left out of `exampleSettingsOfThisBuild()` → the `modules-off` job failed.
- ✅ The setting erased with the two others, and 27/27 server module tests passing in a local `modules-off` build.

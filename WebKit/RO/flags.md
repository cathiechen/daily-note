# How to add flags in WebKit

In generally, there are two ways to create a flag from UI to Core:

- webPreferences + settings(recommanded)
- webPreferences + runtime flags

Settings seems simple, though I've forgoten how to set it. ;) Introduce webpreferences here.

## webPreferences

They are different for each platforms

- WebKit 2
  - UIProcess => WebKit. WebPreferences.yaml, generate the code automatically. Set the flag to WebPreferencesStore. set the flag in store to settings `settings.setResizeObserverEnabled(store.getBoolValueForKey(WebPreferencesKey::resizeObserverEnabledKey()));`
  - WebProcess => WebCore. Use the flag in settings.
- WebKit 1
  - Mac
    - The flag is in WebKitLegacy/.../mac/WebView/WebPreferencesPrivate.h, use WebPreferences::platformUpdateStringValueForKey in WebPreferencesMac.mm to get the flag, and WebPreferences::updateStringValueForKey in WebPreferences.cpp.
  - Windows
    - The flag is in WebKitLegacy/win/WebPreferences.h



Run test cases with flags:
- WebKit 2
  - In FeatureList.pm there is resizeObserverSupport.
- WebKit 1
  - Mac
    - In DumpRenderTree/mac/DumpRenderTree.mm setWebPreferencesForTestOptions get the flag from TestOptions which parse the string "experimental:ResizeObserverEnabled" in the top the test case.
  - Windows
    - In DumpRenderTree/win/DumpRenderTree.cpp enableExperimentalFeatures enable the flag by default, and set the flag to core.
 

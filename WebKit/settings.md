# settings and webfeatures in WebKit

- 弘如何enable:
  - `#if ENABLE(RESIZE_OBSERVER)` ==> `Source/cmake/WebKitFeatures.cmake `
  - `WEBKIT_OPTION_DEFINE(ENABLE_RESIZE_OBSERVER "Enable Resize Observer support" PRIVATE ON)`

- From webPreference to settings
  - webPreference
    - In WebKit, Run in UIProcess
    - inteface: WebPreferences.h
    - implement: webPreferencesPrivate.h(legacy mac), IWebPreferencesPrivate.idl(legacy win stored in CFMutableDictionaryRef), *WebPreferencesExperimentalFeatures.cpp*
    - Input: web
  - settings
    - In WebCore, Run in WebProcess
    - Input: WebPreferenceStore, CFMutableDictionaryRef(if it's legacy win)
- 

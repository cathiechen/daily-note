# About Building

I'm building chrome57 now. Don't ask why. There are some tips worth to be recorded.

## gn

1. Do not use `use_gconf`
2. M57 indicate use gtk2, but only gtk3 installed, so change the BUILD.gn 

```
diff --git a/build/config/linux/gtk/BUILD.gn b/build/config/linux/gtk/BUILD.gn
index bfe9f5841186..bfe00c9a9ae0 100644
--- a/build/config/linux/gtk/BUILD.gn
+++ b/build/config/linux/gtk/BUILD.gn
@@ -30,9 +30,10 @@ group("gtk") {
     public_deps = [
       "//build/config/linux/gtk3",
     ]
-  } else {
+  }
+ else {
     public_deps = [
-      "//build/config/linux/gtk2",
+      "//build/config/linux/gtk3",
     ]
   }
 }
diff --git a/build/config/linux/gtk2/BUILD.gn b/build/config/linux/gtk2/BUILD.gn
index 58af02a9b19c..fcd31fe0ba6a 100644
--- a/build/config/linux/gtk2/BUILD.gn
+++ b/build/config/linux/gtk2/BUILD.gn
@@ -17,7 +17,7 @@ pkg_config("gtk2_internal_config") {
   # misconfigured systems.
   packages = [
     "gmodule-2.0",
-    "gtk+-2.0",
+    "gtk+-3.0",
     "gthread-2.0",
   ]
 }
@@ -35,7 +35,7 @@ group("gtk2") {
 
 # Depend on "gtkprint" to get this.
 pkg_config("gtkprint2_internal_config") {
-  packages = [ "gtk+-unix-print-2.0" ]
+  packages = [ "gtk+-unix-print-3.0" ]
 }
 
```

3. sysroot undefined error, according to newest code, change it to `//build/linux/debian_sid_amd64-sysroot`

```
diff --git a/build/config/sysroot.gni b/build/config/sysroot.gni
index 7fed8b3ee238..bc201d2df0e4 100644
--- a/build/config/sysroot.gni
+++ b/build/config/sysroot.gni
@@ -56,7 +56,8 @@ if (is_linux && target_sysroot_dir != "") {
     sysroot = "//build/linux/ubuntu_precise_amd64-sysroot"
   } else {
     if (current_cpu == "x64") {
-      sysroot = "//build/linux/debian_wheezy_amd64-sysroot"
+      #sysroot = "//build/linux/debian_wheezy_amd64-sysroot"
+      sysroot = "//build/linux/debian_sid_amd64-sysroot"
```

4. import gyp error. Just delete it. The behave is ok for now.


## DEPS

In DEPS, there are many dependence. Ba care of the file name and submit hash code.

Seems `gclient sync` would download them automally. But in my local environment, they need to download manually. Yeah, pretty easy to get wrong.

However, after this, I learnt what DEPS means.

Use this:

```
  'src/v8':
    (Var("chromium_git")) + '/v8/v8.git@a97884ca41aad9c34b2fed891faa2cc2b051e7c7'
```

not this:

```
  'v8_revision':
    '975e9a320b6eaf9f12280c35df98e013beb8f041'

```

## ld error

add `-latomic`.

For instance:


```
--- a/build/toolchain/gcc_toolchain.gni
+++ b/build/toolchain/gcc_toolchain.gni
@@ -316,7 +316,7 @@ template("gcc_toolchain") {
       # .TOC file, overwrite it, otherwise, don't change it.
       tocfile = sofile + ".TOC"
 
-      link_command = "$ld -shared {{ldflags}}${extra_ldflags} -o \"$unstripped_sofile\" -Wl,-soname=\"$soname\" @\"$rspfile\""
+      link_command = "$ld -shared -latomic {{ldflags}}${extra_ldflags} -o \"$unstripped_sofile\" -Wl,-soname=\"$soname\" @\"$rspfile\""
 
       assert(defined(readelf), "to solink you must have a readelf")
       assert(defined(nm), "to solink you must have an nm")
@@ -380,7 +380,7 @@ template("gcc_toolchain") {
         unstripped_sofile = sofile
       }
 
-      command = "$ld -shared {{ldflags}}${extra_ldflags} -o \"$unstripped_sofile\" -Wl,-soname=\"$soname\" @\"$rspfile\""
+      command = "$ld -shared -latomic {{ldflags}}${extra_ldflags} -o \"$unstripped_sofile\" -Wl,-soname=\"$soname\" @\"$rspfile\""
 
       if (defined(invoker.strip)) {
         strip_command = "${invoker.strip} --strip-unneeded -o \"$sofile\" \"$unstripped_sofile\""

```


## compile errors

Most these error is caused by version.

For instance:

1. `readdir_r` is restrited, use `readdir` instead.

2. `interface_base.cpp.tmpl` in charge of generating out/xx/gen/..../V8xxx.h/.cpp. If something wrong, check this file.


```
diff --git a/third_party/WebKit/Source/bindings/templates/interface_base.cpp.tmpl b/third_party/WebKit/Source/bindings/templates/interface_base.cpp.tmpl
index a2532dd525b1..16bb616edd96 100644
--- a/third_party/WebKit/Source/bindings/templates/interface_base.cpp.tmpl
+++ b/third_party/WebKit/Source/bindings/templates/interface_base.cpp.tmpl
@@ -497,17 +497,6 @@ static void install{{v8_class}}Template(v8::Isolate* isolate, const DOMWrapperWo
   {% endfilter %}
   {% endif %}
 
-  {% if interface_name == 'Location' %}
-  // Symbol.toPrimitive
-  // Prevent author scripts to inject Symbol.toPrimitive property into location
-  // objects, also prevent the look-up of Symbol.toPrimitive through the
-  // prototype chain.
-  instanceTemplate->Set(v8::Symbol::GetToPrimitive(isolate),
-                        v8::Undefined(isolate),
-                        static_cast<v8::PropertyAttribute>(
-                            v8::ReadOnly | v8::DontEnum | v8::DontDelete));
-  {% endif %}
-

```



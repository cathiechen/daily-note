# 一些经常用到的命令
## 查看可编译的对象
gn ls out/Release/

## 编译gtest：
gn args out/Release 设置编译变量
ninja -C out/Release content_unittests
run gtest:

## 更新代码：

git pull --rebase && gclient sync

## webkit_unit_tests:
ninja -C out/Release webkit_unit_tests

out/Release/bin/run_webkit_unit_tests --gtest-filter TextAutosizerTest.*

out/Release/bin/run_content_browsertests --gtest-filter File/MediaTest.VideoTulipWebm*

## 编译跑contentshell：
ninja -C out/Release content_shell_apk
build/android/adb_install_apk.py out/Release/apks/ContentShell.apk

## gdb:
build/android/adb_gdb_content_shell --output-directory="out/Release"  --sandboxed=13 --su-prefix="su -c"

## how to log outside webkit?

```
#include "base/logging.h"
LOG(WARNING) << "window_width_dip=" << window_width_dip << "content_width_css=" << content_width_css;


#define AUTOSIZING_DOM_DEBUG_INFO 1

how to log inside webkit?
// deprecated... Use LOG pls :)
#define LAYOUT_AUTOSIZING_LOG(...) WTFLogAlways(__VA_ARGS__)
toLayoutText(layoutObject)->plainText().ascii().data()

void showNode(const blink::Node* node) {
  if (node)
    LOG(INFO) << *node;
  else
    LOG(INFO) << "Cannot showNode for <null>";
}

void showTree(const blink::Node* node) {
  if (node)
    LOG(INFO) << "\n" << node->ToTreeStringForThis().Utf8().data();
  else
    LOG(INFO) << "Cannot showTree for <null>";
}

void showNodePath(const blink::Node* node) {
  if (node) {
    std::stringstream stream;
    node->PrintNodePathTo(stream);
    LOG(INFO) << stream.str();
  } else {
    LOG(INFO) << "Cannot showNodePath for <null>";
  }
}


LOG(INFO) << "cclog GetParentOfFirstLineBox curr_child=" << curr_child;

```

log打不出来怎么办？
重启手机，因为log在跑layouttest的时候被重定向到某个不知名的角落了。。

logcat 过滤：
adb logcat -c && adb logcat chromium:E *:S


## how to set contentshell debuggable?
--- a/content/shell/android/shell_apk/AndroidManifest.xml.jinja2
+++ b/content/shell/android/shell_apk/AndroidManifest.xml.jinja2
@@ -22,7 +22,8 @@
 
     <application android:name="ContentShellApplication"
             android:icon="@mipmap/app_icon"
-            android:label="Content Shell">
+            android:label="Content Shell"
+           android:debuggable="true">
         <activity android:name="ContentShellActivity"
                   android:launchMode="singleTask"

## 跑layouttest:
python third_party/WebKit/Tools/Scripts/run-webkit-tests fast/text-autosizing  -t Default --android --no-pixel-tests

出现permission deny问题的改法：
如果有问题，先改这个文件试试，所有的命令都加上su -c
--- a/third_party/WebKit/Tools/Scripts/webkitpy/layout_tests/port/android.py
+++ b/third_party/WebKit/Tools/Scripts/webkitpy/layout_tests/port/android.py
@@ -729,31 +729,31 @@ class ChromiumAndroidDriver(driver.Driver):
         # Required by webkit_support::GetWebKitRootDirFilePath().
         # Other directories will be created automatically by adb push.
         self._device.RunShellCommand(
-            ['mkdir', '-p', DEVICE_SOURCE_ROOT_DIR + 'chrome'],
+            ['su', '-c', 'mkdir', '-p', DEVICE_SOURCE_ROOT_DIR + 'chrome'],
             check_return=True)
 
         # Allow the test driver to get full read and write access to the directory on the device,
         # as well as for the FIFOs. We'll need a world writable directory.
         self._device.RunShellCommand(
-            ['mkdir', '-p', self._driver_details.device_directory()],
+            ['su', '-c', 'mkdir', '-p', self._driver_details.device_directory()],
             check_return=True)
 
         # Make sure that the disk cache on the device resets to a clean state.
         self._device.RunShellCommand(
-            ['rm', '-rf', self._driver_details.device_cache_directory()],
+            ['su', '-c', 'rm', '-rf', self._driver_details.device_cache_directory()],
             check_return=True)
 
         self._push_data_if_needed(log_callback)
 
         self._device.RunShellCommand(
-            ['mkdir', '-p', self._driver_details.device_fifo_directory()],
+            ['su', '-c', 'mkdir', '-p', self._driver_details.device_fifo_directory()],
             check_return=True)
 
         self._device.RunShellCommand(
-            ['chmod', '-R', '777', self._driver_details.device_directory()],
+            ['su', '-c', 'chmod', '-R', '777', self._driver_details.device_directory()],
             check_return=True)
         self._device.RunShellCommand(
-            ['chmod', '-R', '777', self._driver_details.device_fifo_directory()],
+            ['su', '-c', 'chmod', '-R', '777', self._driver_details.device_fifo_directory()],
             check_return=True)




failed:
http://res.imtt.qq.com/qqbrowser_x5/cathiechen/TextAutosize/text-autosizing/table-inflation-crash.html

备份：加了一些su -c等


third_party/WebKit/Tools/Scripts/webkitpy/layout_tests/port/android.py /big/cathie/android.py.bak


一个解决td的方案：
        // 'm_lastTableColumNum' only effects <td>.
        // Table's colum number impact <td> width.
        // So keep the colnum during last supercluster multipler calculate.
        unsigned m_lastTableColumNum;



## `git cl`的一些命令：
git cl format
git cl upload
git cl config
git cl issue 2299213003


git diff > /big/cathie/autosize_td.diff
git apply /big/cathie/autosize_td.diff


## Gtest跟su相关的修改：
third_party/catapult/devil/devil/android/device_utils.py
--- a/devil/devil/android/device_utils.py
+++ b/devil/devil/android/device_utils.py
@@ -912,7 +912,7 @@ class DeviceUtils(object):
                                     cmd_helper.SingleQuote(cmd))
     if as_root and self.NeedsSU():
       # "su -c sh -c" allows using shell features in |cmd|
-      cmd = self._Su('sh -c %s' % cmd_helper.SingleQuote(cmd))
+      cmd = self._Su(' %s' % cmd_helper.SingleQuote(cmd))





## trace:
  1. TRACE_EVENT0("blink,benchmark", "TextAutosizer::markSuperclusterForConsistencyCheck");
  
  2. TRACE_EVENT_BEGIN1("blink,devtools.timeline", "UpdateLayoutTree", "beginData",
                     InspectorRecalculateStylesEvent::Data(GetFrame()));

  TRACE_EVENT_END1("blink,devtools.timeline", "UpdateLayoutTree",
                   "elementCount", element_count);

## 有用的log：
BlockPainter::intersectsPaintRect
    //WTFLogAlways("AnonymousBlock=%p, BlockPainter::intersectsPaintRect overflowRect(%d, %d, %d, %d) after addElementVisualOverflowRects",
    //              &m_layoutBlock, overflowRect.x().toInt(), overflowRect.y().toInt(), overflowRect.width().toInt(), overflowRect.height().toInt());

  //WTFLogAlways("%p, BlockPainter::intersectsPaintRect overflowRect(%d, %d, %d, %d) after unite visualOverflowRect",
  //            &m_layoutBlock, overflowRect.x().toInt(), overflowRect.y().toInt(), overflowRect.width().toInt(), overflowRect.height().toInt());


## 如何带flag跑起content-shell

- `build/android/adb_run_content_shell --enable-logging --v=1`
- `adb shell am start -n org.chromium.content_shell_apk/.ContentShellActivity -es --v=1`
- ` adb shell 'echo "chrome --enable-logging --v=1" > /data/local/tmp/content-shell-command-line'` 然后在跑content-shell


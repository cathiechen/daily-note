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
- build/android/adb_gdb_content_shell --output-directory="out/Release"  --sandboxed=13 --su-prefix="su -c"
- build/android/adb_gdb_content_shell --output-directory="out/Debug"  --start --su-prefix="su -c"  --verbose
- 如果不能跑，试试把contentshell的debuggable设置成true。

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

[layouttest redirect](https://codereview.chromium.org/2540603004/): 这个patch。。。
- 把原来layout test的“fifo”逻辑去除了。
	- 原来的逻辑：`base::android::CreateFIFO(path, 0666)`
	- `base::android::RedirectStream(stdout, stdout_fifo, "w")`
	- 这样就可以把stdin, stdout, stderr重定向到path文件里面，这样就可以通过这些fifo进行交互
- 新逻辑：
	- 主线程给IO线程发送一个task，让io线程执行`CreateAndConnectSocket()`,闭包（`base::Bind`）你还记得吗？然后就等待消息
	- IO线程执行完`CreateAndConnectSocket()`后，发送signal回来
		- `CreateAndConnectSocket()`： 创建socket.
			```
			  net::CompletionCallback connect_completed =
      				base::Bind(&ConnectCompleted,
                 		base::Bind(socket_connected, base::Passed(std::move(socket))));
			```
		- `socket_connected`是主线程传过来的闭包，
			- FinishRedirection，socket创建完毕，保存socket，重置fd
				- add_socket:把socket添加到`ScopedAndroidConfiguration`中的socket_数组里面
				- RedirectStdout： `dup2(fd, STDOUT_FILENO)`, 大概这样就把stdout重定向了吧。。。
		- `result = socket_ptr->Connect(storage, connect_completed);`执行完后，执行`connect_completed`

	- 主线程创建socket完毕，继续
	- destruct = default， 这样会把数组中的socket删除，但stdout的fd没被重置回来，会导致跑完layout test后，log不知道打到哪里去的问题。
stdin, stdout, and stderr are 0, 1,and 2

logcat 过滤：
`adb logcat -c && adb logcat chromium:E *:S`


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

结果所处位置：src/out/Debug/layout-test-results


python third_party/WebKit/Tools/Scripts/run-webkit-tests fast/text-autosizing  -t Default --android --no-pixel-tests

出现permission deny问题的改法：
如果有问题，先改这个文件试试，所有的命令都加上su -c
--- a/third_party/WebKit/Tools/Scripts/webkitpy/layout_tests/port/android.py
+++ b/third_party/WebKit/Tools/Scripts/webkitpy/layout_tests/port/android.py
         self._device.RunShellCommand(
-            ['chmod', '-R', '777', self._driver_details.device_fifo_directory()],
+            ['su', '-c', 'chmod', '-R', '777', self._driver_details.device_fifo_directory()],
             check_return=True)

改变server的路径,确保相应目录上有对应的test case
--- a/third_party/WebKit/Tools/Scripts/webkitpy/layout_tests/port/driver.py
+++ b/third_party/WebKit/Tools/Scripts/webkitpy/layout_tests/port/driver.py
@@ -262,7 +262,8 @@ class Driver(object):
 
         if '/https/' in test_name or '.https.' in test_name:
             return 'https://%s:%d/%s' % (hostname, secure_port, relative_path)
-        return 'http://%s:%d/%s' % (hostname, insecure_port, relative_path)
+               #return 'http://%s:%d/%s' % (hostname, insecure_port, relative_path)
+        return 'http://res.imtt.qq.com/qqbrowser_x5/cathiechen/LayoutTests/%s' % (hostname, insecure_port, relative_path)



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

修改本地log：
git commit --amend


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


# ROOT N7

搭建telemetry的手机要求安装的是userdebug版本的rom。即要可以运行： adb root。telementry在已root的s4上跑出来的结果很不稳定。没办法，只好root n7.

一遇到root，刷机，搭环境智商就降到0.:( 这次也不例外。。好多坑，先记下来

## 命令行

一开始用命令行搞，差点把手机搞成砖。还是记录一下吧：

1. 下载android studio， 为了用里面的d:\soft\android\skd\platform-tools\androidadb.exe和fastboot.exe

2. `adb reboot bootloader` `fastboot oem unlock` 主要是unlock

3. 然后又下了个img， `fastboot flash recovery xxx.img`

4. 又下载了一个supersu, 并把这个zip push到手机中。运行recovery时，安装supersu。

## nexus root toolkit

后来发现有NRT这个工具，UI界面让人开心了很多。一开始先选手机，以及要安装的rom。等一会儿，好像在下载文件什么的。弹出一个界面，界面上可以直接解锁，root。root时会先去下载img，但下载不成功，可以直接网络上下载下来，再通过本地选。

这个工具很好，nexus xx的手机都可以用。 好棒！

## adbd-Insecure.apk

虽然root了，但还是没法运行`adb root`。可能NRT上的都不是userdebug版本。快要放弃的时候，看到adbd-insercure.apk。安装后，可以直接`adb root`了。

## telemetry终于在n7上跑起来了

一开始还是不行，什么`getprop service.adb.root`获取不到直，索性在脚本里把调用的地方return true.. 终于成功跑起来了～～ 


最后，n7上跑出来的结果比s4稳定很多。可惜html5-full-render变化还是很小，没法重现bug。


# jdk相关

总的说来是想升级一下jdk。原因呢？是编译的时候经常遇到下面的错误：

	warning: lib.java/android.interface.jar(java/lang/AutoCloseable.class): major version 52 is newer than 51, the highest major version supported by this compiler.
  	It is recommended that the compiler be upgraded.

## 查看jdk所在目录

首先，有记忆以来，我都没有安装过jdk。各种k傻傻分不清楚。所以，只好厚着脸皮问同事。
同事有下面命令非常巧妙地找出我的jdk：

==
```
@cc:~/work/jdk$ which java
/usr/bin/java
cc@cc:~/work/jdk$ ll /usr/bin/java
lrwxrwxrwx 1 root root 22 Dec 20  2012 /usr/bin/java -> /etc/alternatives/java*
cc@cc:~/work/jdk$ ll /etc/alternatives/java
lrwxrwxrwx 1 root root 46 Jun 27 17:20 /etc/alternatives/java -> /usr/lib/jvm/java-7-openjdk-amd64/jre/bin/java*
```
==

## 安装jdk：

[网上下载jdk](http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html)

## 解压并配置环境变量
- 解压命令： tar -xvf jdk-8u111-linux-x64.tar.gz
- 环境变量配置： vi ~/.bashrc， PATH上加上： /home/cc/work/jdk/jdk1.8.0_111

## 更新环境变量
- source ~/.bashrc
- or new a command window

## vim的设置
不知道为啥，最近vim打开就有部分中文是乱码。网上查了解决方法，终于有个可以用了。
```
sudo vim /etc/vim/vimrc
在该文件最后加上：set encoding=utf-8
```

## ubantu电脑桌面断电后坏了
- 输入登录密码后，只显示桌面和鼠标。
  - 原因：unity出错，需要在设置界面内（ccms: compizconfig-settings-manager）把它打开
  - 解决方法：
    - 打开tty1： Ctrl + alt + f1
    - 重新安装：`ubuntu-desktop`，`compizconfig-settings-manager`
    - 输入： `DISPLAY=:0 ccsm`, `startx`
    - Ctrl + alt + f7/f8，此时应该可以看到桌面，并有ccsm界面弹出，勾选unity，重启。
- locale: 
  - 现象
    - 小问题：cssm运行的时候会提示错误什么`LC_Type`没设置， terminal输入`locale`有错误提示
    - 大问题：locale设置要小心，什么`LC_ALL`之类的，是不能随便设置的。有可能会导致进不了系统，只能进入recovery model。吓shi姐了，真的！
  - 解决方案：
    - [参考文档](https://wiki.archlinux.org/index.php/Locale_)
    - `locale`：看当前的设置

	```
		cc@cc:~$ locale
		LANG=en_US.UTF-8
		LANGUAGE=en_US:en
		LC_CTYPE="en_US.UTF-8"
		LC_NUMERIC="en_US.UTF-8"
		LC_TIME="en_US.UTF-8"
		LC_COLLATE="en_US.UTF-8"
		LC_MONETARY="en_US.UTF-8"
		LC_MESSAGES="en_US.UTF-8"
		LC_PAPER="en_US.UTF-8"
		LC_NAME="en_US.UTF-8"
		LC_ADDRESS="en_US.UTF-8"
		LC_TELEPHONE="en_US.UTF-8"
		LC_MEASUREMENT="en_US.UTF-8"
		LC_IDENTIFICATION="en_US.UTF-8"
		LC_ALL=
	```

    - `locale -a`: 看当前有的字库？
    - 若有些字库没下载下来：`sudo vi /etc/locale.gen`
    - `locale-gen`: 生成locale
    - 设置环境变量：`/etc/environment` 或 `/etc/default/locale`。只有我觉得环境变量好乱吗？一定是我了解得不够！
      - `/etc/environment`：`LANG="en_US.UTF-8"`
      - `/etc/default/locale`: 
	
	```
		LANG=en_US.UTF-8
		LANGUAGE="en_US:en"
	```

- `gnome-panel`:侧边栏出来了，上面的还是出不来了
  - 原因：大概是一运行起来就crash了，或运行了一会儿后crash了。。
  - 解决：
    - 尝试重新安装，开机启动：` cp /usr/share/applications/gnome-panel.desktop ~/.config/autostart/`，但都没有用，放弃：（
    - 临时解决方案：
      - alt + f2： 启动`gnome-panel`，这种方法不能长久显示，还是显示一会儿后，就消失了。
      - command： `pkill gnome-panel`, `gnome-panel`。要启动一个terminal，还是会消失，此时重复以上就好了。
- 中文输入法安装：
  - 小tu，输不了中文，要你何用？
  - 解决：
    - 安装language selector：`sudo apt-get install language-selector-gnome`
    - 安装拼音：`sudo apt-get install fcitx fcitx-googlepinyin im-config`
    - 输入：`im-config`，选fcitx，
    - 重启
    - 在输入法设置中把拼音输入法选上，
- 开机非比现黑屏
  - 不确定原因，网上很多说是显卡的问题，但是怀疑一下
  - 解决：
    - 减少开机，多保存，不只是本地保存啊。
    - 若开机黑屏：可以等一会儿; Ctrl + alt + f1 ,  startx，可行。
我和linux的问题在于，网上搜到的命令，迫不及待想输进去看看，不知这个命令啥意思。感觉很恐怖。


## 网络
如果ifdown不能用，可以试试：sudo ifconfig eth0 down
up，同理

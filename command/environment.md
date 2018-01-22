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
进入图形界面的command：startx
推出图形界面：ctrl + alt + backspace
ctrl + alt + T


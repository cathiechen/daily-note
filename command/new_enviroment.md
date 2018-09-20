# 新环境搭建


## linux

1. 磁盘挂载：

由于chrome是个很大的工程，所以在所有动作开始之前，先确保磁盘空间充足。新装的linux系统，文件系统只有系统盘/dev/sda，大概就100G。千万不要直接下代码，不然空间肯定不足。所以，第一个就是要挂载磁盘。

- 如何挂载磁盘：
  - `df -h`查看文件系统情况，只有100多g
  - `sudo fdisk -l`查看磁盘情况，发现/dev/sdb，有900多g
  - `sudo fdisk /dev/sdb`，可以进行分区啥的
  - `sudo mount /dev/sdb5 ./disk`， 把sdb5 mount到disk上，这是临时的，可以跳过，记得修改disk的chown
  - 永久mount，`sudo vi /etc/fstab`，按照规则添
  - If U wanna creat an ext4 filesystem: `sudo mkfs.ext4 /dev/sdb5`  
  - 参考1： 鸟哥
  - 参考2： [linux下磁盘分区、挂载知多少](https://www.jianshu.com/p/ce31ae7da616)


2. package lists更新： `sudo apt-get update`

可能还要加各种源？可以在"software source" app上，"ubuntu software" tab上看看。

另外在[packages.ubuntu.com](https://packages.ubuntu.com)可以查看，某个package是否存在。 

3. 安必要的工具： `sudo apt-get install vim/net-tools/silversearcher-ag`

4. 下载下来的新的代码，莫名其妙就是报错？可以去相应的目录，`git pull` :)


## windows

1. 安装：
   1. foxmail， 搜狗输入法，chrome, 企业微信等等
   2. sublime text， edraw max, typora, slack 等等
2. 拷贝旧电脑的东西到新电脑
   1. 照片
   2. 工具：vs， va等等
   3. 文档，文档存放要合理哇~~~
3. 账号
   1. slack：https://chromiumdev.slack.com/
   2. chromerenderingcore.slack.com

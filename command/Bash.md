# Bash

## 一些位置

- `/etc/shells`中存在本机可以使用的shell， bash是众多shell的一种

- 上次及以前用过的bash指令，保存在：`~/bash_history`中

## 一些命令

- 【tab】在第一个单词是补全命令，第二个是补全路径

- 别名的设置，可以用来减少输入，如：alias lm='ls -al'

- `type`：查询命令的类型，如： `type ls`

- [ctrl]+u/k，删除前后指令

- [ctrl]+a/e，光标移动到最前或最尾

## 变量

  - bash的变量设置时：`PATH=xxx` ;  读取时， `echo $PATH`

    - 不能带空格， 若赋值多个word， “” 代表使用执行特殊字符， ‘’代表只是文字输入

    - 用‘\’将特殊字符，变成字元？

    - 执行其他指令： 『version=$(uname -r)』再『echo $version』可得『3.10.0-229.el7.x86_64』

    - 变量累加（类似+=）： 『PATH="$PATH":/home/bin』或『PATH=${PATH}:/home/bin』

    - 利用export贡献变量： 『export PATH』，如： name=cc , $name这个变量如果想在其他shell中应用，则需要: export name

    - 大写的是系统变量；小写的是自己定义的

    - 取消变量设置：『unset myname』

    - 这个很好，用起来：

      ```````
      [dmtsai@study ~]$ work="/cluster/server/work/taiwan_2015/003/"
      [dmtsai@study ~]$ cd $work
      ```````

      ​

  - echo 输出变量内容

  - env看已经设置的环境变量，$RANDOM随机

  - set 看变量赋值



## 关于命令

- `man -k`搜索命令


- 简要说明`whatis`； 详细说明`info`
- 命令位置（程序位置）`which`
- 具体详细说明：`man`
- 查看多个版本信息： `whereis`

## 文件处理

- `ls | cat -n` 列出文档，并加上编号
- `find`
  - `find ./ -name "xx*" | xargs file`: 找到已xx开头的文件名，并把这些文件名当成参数传给file
  - `find ./ -name "*.o"`
  - `find ./ -name "xx*" -exec ls {} \;` , `find -type f -exec ls -l {} \;` exec代表执行，{}代表find出来的内容，一定要有\;
- `locate`非实时查找，先建数据库(`updatedb`)，然后直接在数据库中查找(`locate xx`).
- 查看文件
  - `cat -n`， 直接显示
  - `ls -al | more`, `more`分页显示
  - `head - 10 xx` 偷看前十行
  - `tail - 5 filename`, `tail -f xxx.log`显示末尾最新的内容
  - `diff xx1 xx2`
- 查找文件内容 `egrep`
- 文件别名：`ln xx xxagain` 硬连接，可删除； `ln -s xx xxagain`软连接，不可删除xx
- 管道和重定向

## ubuntu的软件更新
一直不是很清楚，尽量把自己理解的记录下来，不一定对，但下次就不要再从头开始了。
不同的包放在不同的源，设置源地址的方法：
- sudo vi /etc/apt/sources.list 里加上类似`deb http://security.debian.org/debian-security jessie/updates main `的东西
- 设置 软件更新 可以通过ui设置。
`sudo apt-get update` 更新
设置完后，可以`sudo apt-get install libnss3`安装软件

btw用history可以查看之前的输入历史。

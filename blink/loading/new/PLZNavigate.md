# [PLZNavigate](https://docs.google.com/presentation/d/1B6CpibtGJEV2zdtuyVNsF_OOabwVKIKGRZ3WuHUUUv4/edit#slide=id.g1bc511d4c5_0_0)

这个优化主要的点就是，创建一个render进程/线程消耗的时间，根据平台有所不同。当然，还让逻辑更加简单了。

## 现状
- 由render线程/进程发起网络请求
- 然后由browser线程/进程处理这些请求
- 等待render线程/进程创建完后，才可以发起请求

## 架构调整
- 简单地，直接有browser发起请求
- 减少render和browser之间的nav交互

## 对比
### 目前：
- 发起请求的时候，若没有render，创建render，然后由render发起请求
- browser收到请求，发起网络请求
- render显示

### 调整后：
- 直接通过browser发起请求，不需要先创建render
- 发起后，找一个render来显示，如果没有，创建一个render

## 流程拆分
- nav开始
	- 请求马上有browser发给IO
	- 若没有render，创建一个
	- 这样可以实现render和发起请求的并行
- commiting nav
	- 选一个render
	- render显示时，已跳过所有的redirect
- nav has committed
	- no change

## 现状
- 上线测试阶段

## 后续
- 改成mojostream
- 去除一些类

#TextAutoSize

##作用

##如何实现

##后续方向

##不一致问题
- 主要思路
- 需要解决的问题
	- supercluster嵌套问题
		- 解决思路1：用最外层supercluster，但如果外层不需要放大，内层需要放大就会存在问题。
		- 解决思路2：添加所有的supercluster,先遇到内层，还是先遇到外层，这是个问题。
		- 解决思路3：用第一次碰到的非inheritmultiplier的supercluster，或最后一个碰到的supercluster
	- TD fingerprint不可信问题

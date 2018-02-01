# TextAutoSize

## 作用

## 如何实现
- 每个document都有一个autosizer，autosizer创建: document initialize.
- page info。page info决定是否进行主体字放大，包含以下信息: frame size, settings, css: adjust等
  - 引起update pageinfo的触发点：document initialize; frameview resize; settings change
  - update page info 做了什么事情： set all text needs layout. or reset multiplier

- record 
  - 触发：每个obj的styledidchanged
  - record做了什么事情：
    - classify block: 将blocks分成4类：
      - is potentialClusterRoot：基本就是非空的block
      - independent：table/td/table capital, multicolumn, layoutview, float, position, flex, diff write mode, replaced, textarea, usermodify != readonly
      - explicit width
      - suppress： 直接孩子是form control; row of link（a链接的字数，字体一样，没有`<br>`）; `white-space: no-wrap`; 有设置高度
    - 计算finger print，并保存
      - independent 和 explict width类的节点，计算finger print
      - finger print是什么？ 是hash：parent finger print + tag name + direction + position + float + display + width > compute hash
      - finger print 和 block的存储：存储在两个hash map中，key: finger print; value: block set. key: block; value: finger print.
    - check 是否新增节点，若是新增节点，标志其super cluter需要check insistent

- layout scope:
  - 创建： before layout
  - 功能：
    - beginlayout：
      - 对此次layout pass 的第一个节点：prepareforlayout: PrepareClusterStack其实是准备cluster stack。对每个祖先节点MaybeCreateCluster
        - 什么样的节点要创建? cluster:diff suppresses; independent; explicit width
        - cluster包括：flag(independent), block, parent cluster, supercluster,deepest block containing all text, multiplier, has enough text等
        - super cluster: fingerprint相同的root大于等与2个。super cluster包括: roots， has enough text, multiplier, isinheritmulitiplier.
      - 若是layout view节点，此时应该check super cluster consistent. 遍历所有被mark的supercluster，重算multiplier，若multiplier发生变化，对这个supercluster set all text needs layout. clear marked supercluster list.
    - 对当前block：MaybeCreateCluster
    - inflate
       - 对inline 节点递归到text节点。计算multiplier
          - cluster multiplier
            - 非independent, 跟本cluster的宽度与父亲cluster的宽度差别不大（0 < width diff < 200），使用父亲cluster的multiplier
            - 其他，用本cluster的multiplier
              - 没有supercluster,字数足够：字数 * fontsize > providerwidth * 4, 计算放大倍数：multiplier = provider width / frame width
              - provider width: the logical width of bock which contains all text. deepest block contains all text.
              - 有super cluser 使用supercluster的multiplier。supercluser在遇到第一个需要计算multiplier的text时，把名下所有的block都遍历一下，判断这些block里面的字数是否足够，若足够则计算multiplier。然后，这个multiplier被共享。
      - apply multiplier
  - delete layout scope: after layout:
    - `cluster_stack_.pop_back();`


## 后续方向

## 不一致问题
- 主要思路
- 需要解决的问题
	- supercluster嵌套问题
		- 解决思路1：用最外层supercluster，但如果外层不需要放大，内层需要放大就会存在问题。
		- 解决思路2：添加所有的supercluster,先遇到内层，还是先遇到外层，这是个问题。
		- 解决思路3：用第一次碰到的非inheritmultiplier的supercluster，或最后一个碰到的supercluster
	- TD fingerprint不可信问题

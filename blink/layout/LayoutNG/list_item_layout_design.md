# Layout List Item

outside List marker 最大的矛盾是：
1. 要根据孩子节点或后面的兄弟节点的排版位置和宽高来决定自己的margin(水平方向的位置)和position。
2. 不能影响后面兄弟节点的排版。

outside marker只是想安静地当个bullet而已。:)
但以上两点都跟正常排版流程是相背的，所以很麻烦。

由于跟后面的节点layout结果相关，所以创建list item的时，加了subtreechanged的监听！

## 现版layout如何处理这两个问题

marker是个inline的类似replace的节点。这样避免了，其他节点插入marker的可能性。

- 要根据孩子节点或后面的兄弟节点的排版位置和宽高来决定自己的margin和vertical position
  - margin：在创建marker时，计算marker里面内容的宽度，根据该宽度计算margin。所以marker作为类似replace的节点，还有这个功能，可以自己计算自己的宽度。
    - text：根据font metrics计算
    - img：根据image的size计算
  - vertical position：
    - original：将marker插入到li产生inlinebox的孩子节点前面，这样可以避免垂直位置的计算，但存在问题。
    - new：marker独立加入到一个anonymous block，等兄弟节点layout之后，对齐垂直位置
  - horizontal position：
    - 如果存在float入侵的情况，水平移动marker的位置

- 不能影响后面兄弟节点的排版
  - origial：插入到li产生inlinebox的孩子节点前面，加上margin的计算，这里可以不处理。然而，有存在无法修改的问题。
  - new：marker的container的高度设置成0px。lol，我想出来的！

## 全新版本应该是怎样的？
LayoutNG是可以碎片化的啊，应该可以随时随地构建一个NGNode，layout，得到layoutresult才是，对吧？
所以，如何解决这些问题：

- 要根据孩子节点或后面的兄弟节点的排版位置和宽高来决定自己的margin和vertical position
  - margin：需要margin吗？直接`height: 0px`，layout的时候，水平方向移动会怎样？
    - 激进的做法：创建一个anonymouse block为marker，设置其高度为0px。marker内不能插入其他孩子节点！！这个扩展性不好，再想想。
  - vertical position：
    - list item layout到marker的时候，先向后layout 其他孩子节点，找到内容后，继续layout marker。marker正常layout，之后，根据后面内容移动自己的位置，水平和垂直位置一起。
  - horizotal position：
    - 见vertical position。
- 不能影响后面的兄弟节点排版
  - 不会！！！

这样做会有什么问题吗？


## ng上的实现

啊啊啊，我一点都不想放假。。。

- 创建一个height 0的marker，然后给它创建内容
- block node layout时，对marker隔离处理：等所有兄弟节点layout完后，再layout marker
- inline node layout时，判断如果是marker，对marker 的line box直接左移和垂直对齐（目前还没实现）

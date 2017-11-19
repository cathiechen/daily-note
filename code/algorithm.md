# 算法

主要记录算法相关笔记。

## 算法导论

### 分治策略

分治的思想主要是：先把大问题分成小问题，逐个解决小问题，然后再合并，得出大问题的解。

分3步：

1. 问题分解：大问题分解成小问题，最好等分（如：二分法），这样效率才高
2. 小问题解：小问题就是基本问题，就是递归前面的那个if
3. 合并解：小问题的解合并成大问题的解，这应该是最难的部分了

难点：

1. 具体问题的抽象化，比如，问题该怎样转换成适合分治策略模型。比如：股票买卖最高获利问题，转成：最大连续子数组和最大问题。其实，抽象化最主要的工作就是，找对数据结构。不是说，用数组还是链表这种的；是说，如何把问题中的数据，很顺地表示出来。如: 股票的例子中，把源数据股票价格，改成，存储第i天价格-第i-1天的价格差。这是一个很好的，抽象的例子
2. 小问题是否把大问题所有的情况都概括到了

例子：

1. 最大连续子数组和
   - 分解： 数组等分，左边最大子数组和，加右边；横跨中间元素的，最大子数组和
   - 小问题解： 递归就不说了。横跨中间元素的解：中间位到低位最大值+中间位到高位最大值
   - 合并： 取三者中最大的值
   - 啰嗦：这个问题中解返回的数据结构很好啊!`(left_position, right_position, max_sum)`
2. 更多例子补充



### 概率问题

跟着书本走，分治后面是概率。。

概率问题：

给出一个策略，让你估计这个策略大概会是怎样的结果。

例子：

招聘的例子，给一个面试名单，如果当前面试的人比已经面试的都好，雇佣该面试人，解雇以前面试的同学。请问，这个过程大概会雇佣几个人？

这样分析：第一个人被雇佣的概率是1， 第二个比第一个好的概率是1/2， 第三个是最好的概率1/3，第四个...

所以就是i=1->n的1/i的累加。

另一个方法，还没看懂，明天继续！

### 堆

这是一种类似class的数据结构，也就是除了存储，还提供很多函数来维护这个堆。

- 改： lgn
- 排序：nlgn
- 最大值：1
- 消耗内存：n

具体实现：

- 它基本是一个数组，但会提供另外两个成员变量: length(数组大小) 和 heap_length（堆的长度）
- 提供的函数：
  - Parent(i), LeftChild(i), RightChild(i)
  - MaxHeapify(i)
  - BuildMaxHeap()
  - HeapSort()
  - HeapMaximum()
  - HeapExtractMax()
  - HeapIncreaseKey(i, x)
  - MaxHeapInsert(x)


- 实现

```
class MaxHeap {
  public:
    MaxHeap() {
  	  length_ = 21;
  	  heap_length_ = 0;
    }
    
    int Pearent(int pos) {return pos / 2;}
    int LeftChild(int pos) {return pos * 2;}
    int rightChild(int pos) {return Pos * 2 + 1;}
    
    void MaxHeapify(int pos) {
      int max_pos = pos;
      int max = array_[pos];
      if (Left(pos) < length && max < array_[Left(pos)]) {
        max = array_[Left(pos)];
        max_pos = Left(pos)
      } else if (Right(pos) < length && max < array_[Right(pos)]) {
        max = array_[Right(pos)];
        max_pos = Right(pos)
      }
      if (max_pos != pos) {
        array_[max_pos] = array_[pos];
        array_[pos] = max;
        MaxHeapify(max_pos);
	  }
    }
    
    void BuildMaxHeap() {
      heap_legnth_ = length_;
      for (int i = length / 2; i > 0; i--) {
        MaxHeapify(i);
	  }
    }
    
    void HeapSort() {
      heap_legnth_ = 0;
      for (int i = length; i > 0; i--) {
      	int tmp = array_[i];
        array_[i] = array[1];
        MaxHeapify(0);
      }
    }
    
    int HeapMaximum() {return array_[1];}
    
    int HeapExtractMax() {
      int max = array_[1];
      array_[1] = array_[heap_length_]
      heap_legnth_--;
      MaxHeapify(0);
      return max;
    }
    
    void HeapIncreaseKey(int pos, int value) {
      if (pos > length_ || array_[pos] >= value)
        return;
      array_[pos] = value;
      if (value > array_[Parent(pos)]) {
        array_[pos] = array_[Parent(pos)];
        HeapIncreaseKey(Parent(pos), value);
      }
    }
    
    void MaxHeapInsert(int x) {
      heap_legnth_++;
      array_[heap_length_] = -10000;//-...
      HeapIncreaseKey(heap_legnth_, x);
    }
    
  private:
    int array_[21];
    int length_;
    int heap_length_;
};
```

- 使用堆时的不方便
  - length_ 和 heap_length_ 的维护。length_ 代表array_ 的长度， heap_length_ 代表已经在堆中的长度。sort应该放在最后。也就是，如果：build，sort，再insert，这样length_ 和已经sort的元素会混乱。所以，正确顺序，应该是： build, insert, 确定不会再有新加元素了，在sort；或者把已经sort好的元素拷贝其它地方，这样新加的元素和已经sort的元素就没在一起排序了；或者再重新build，再insert，再sort。总之，最好是确定了基本不会再增加元素了，再sort。
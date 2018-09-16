# c++

天知道我有多菜！！TT

## stl

[这篇。。。](http://net.pku.edu.cn/~yhf/UsingSTL.htm)

### 1. 迭代器

对it的操作：

`  int* ip = find(iarray, iarray + SIZE, 50);`

`find(intVector.begin(), intVector.end(), 50);`



`const vector<int>::iterator it;`



`copy(darray, darray + 10, outputIterator);`



`replace(vdouble.begin(), vdouble.end(), 1.5, 3.14159);`



`reverse(vdouble.begin(), vdouble.end());`



`random_shuffle(vdouble.begin(), vdouble.end());`



`  sort(collection.begin(), collection.end());`



`  copy(v.begin(), v.end(), ostream_iterator<int>(cout, "\t"));`



`  copy(iArray, iArray + 2, inserter(iList, it)); `



`advance(it, 2);  // same as it = it + 2;`



```c++
distance(it, iList.end(), k);
```

 

### 2. 函数和函数对象



```
void show(const long &ri);
for_each(v.begin(), v.end(), show);


bool isMinus(const long &ri);  // Predicate function
p = find_if(v.begin(), v.end(), isMinus);//调用断言函数


TAnyClass object;  // Construct object
object();          // Calls TAnyClass::operator()() function
for_each(v.begin(), v.end(), object);

accumulate(v.begin(), v.end(), 0);
accumulate(v.begin(), v.end(), 1, multiplies<long>());
```

## 智能指针

[c++智能指针](https://www.jianshu.com/p/68fc49d55374)

1. std::auto_ptr  不推荐使用

2. std::unique_ptr 只允许一个实例管理一块内存

   ```
   std::unique_ptr<Fraction> f2; // 初始化为nullptr
   auto f3 = std::make_unique<Fraction>(2, 7);
   takeOwnerShip(std::move(ptr));
   ```

3. std::shared_ptr 有个引用计数，实现时用指针，md

   ```
   auto ptr1 = std::make_shared<Resource>();
   ```

   

4. std::weak_ptr 防止std::shared_ptr互相引用，无法删除的问题

   ```
   std::weak_ptr<Person> m_partner;
   ```

   

## c++11

[高速上手c++11/14](https://changkun.gitbooks.io/cpp1x-tutorial/content/) 这个真的写得很好。

### 语言可用性强化

1. nullptr

2. constexp:  

   ```
   constexpr int sum(int a, int b) {
       return a + b;
   }
   
   int a[sum(10+10)];
   ```

3. 类型推导： auto

4. decltype: decltype(x+y) add(T x, U y)

5. for and auto: 

   ```
   int array[] = {1,2,3,4,5};
   for(auto &x : array) {
       std::cout << x << std::endl;
   }
   ```

6. initializer_list， 用{}初始化对象

   ```
   #include <initializer_list>
   
   class Magic {
   public:
       Magic(std::initializer_list<int> list);
   };
   
   Magic magic = {1,2,3,4,5};
   
   std::vector<int> v = {1, 2, 3, 4};
   ```

7. 别名using

   ```
   typedef int (*process)(void *);  // 定义了一个返回类型为 int，参数为 void* 的函数指针类型，名字叫做 process
   using process = int(*)(void *); // 同上, 更加直观
   using NewType = SuckType<std::vector, std::string>;
   ```

8. 默认模板参数

   ```
   template<typename T, typename U>
   auto add(T x, U y) -> decltype(x+y) {
       return x+y
   }
   ```

9. 变长参数模板

10. 委托构造

    ```
    class Base {
    public:
        int value1;
        int value2;
        Base() {
            value1 = 1;
        }
        Base(int value) : Base() {  // 委托 Base() 构造函数
            value2 = 2;
        }
    };
    ```

11. 继承构造

    ```
    class Subclass : public Base {
    public:
        using Base::Base;          // 继承构造
    };
    ```

12. override and final

13. 显示禁用默认函数

    ```
    class Magic {
    public:
        Magic() = default;  // 显式声明使用编译器生成的构造
        Magic& operator=(const Magic&) = delete; // 显式声明拒绝编译器生成构造
        Magic(int magic_number);
    }
    ```

14. 强类型枚举类

    ```
    enum class new_enum : unsigned int {
        value1,
        value2,
        value3 = 100,
        value4 = 100
    };
    ```

### 运行期间的强化

1. lambda 表达式

      ```
   [捕获列表](参数列表) mutable(可选) 异常属性 -> 返回类型 {
       // 函数体
   }
   
   void learn_lambda_func_1() {
       int value_1 = 1;
       auto copy_value_1 = [value_1] {
           return value_1;
       };
       value_1 = 100;
       auto stored_value_1 = copy_value_1();
       // 这时, stored_value_1 == 1, 而 value_1 == 100.
       // 因为 copy_value_1 在创建时就保存了一份 value_1 的拷贝
   }
   
   
      ```

   - [] 空捕获列表
   - [name1, name2, ...] 捕获一系列变量
   - [&] 引用捕获, 让编译器自行推导捕获列表
   - [=] 值捕获, 让编译器执行推导应用列表

2. 函数包装器

   ```
   #include <iostream>
   
   using foo = void(int);  // 定义函数指针, using 的使用见上一节中的别名语法
   void functional(foo f) {
       f(1);
   }
   
   int main() {
       auto f = [](int value) {
           std::cout << value << std::endl;
       };
       functional(f);  // 函数指针调用
       f(1);           // lambda 表达式调用
       return 0;
   }
   
   
   int foo(int para) {
       return para;
   }
   std::function<int(int)> func = foo;
   func(10);
   ```

   std::bind和std::placeholders

   ```
   
   int foo(int a, int b, int c) {
       ;
   }
   int main() {
       // 将参数1,2绑定到函数 foo 上，但是使用 std::placeholders::_1 来对第一个参数进行占位
       auto bindFoo = std::bind(foo, std::placeholders::_1, 1,2);
       // 这时调用 bindFoo 时，只需要提供第一个参数即可
       bindFoo(1);
   }
   ```

3. 右值引用

   **左值(lvalue, left value)**，顾名思义就是赋值符号左边的值。准确来说，左值是表达式（不一定是赋值表达式）后依然存在的持久对象。

   +

   **右值(rvalue, right value)**，右边的值，是指表达式结束后就不再存在的临时对象。

   而 C++11 中为了引入强大的右值引用，将右值的概念进行了进一步的划分，分为：纯右值、将亡值。

   **纯右值(prvalue, pure rvalue)**，纯粹的右值，要么是纯粹的字面量，例如 `10`, `true`；要么是求值结果相当于字面量或匿名临时对象，例如 `1+2`。非引用返回的临时变量、运算表达式产生的临时变量、原始字面量、Lambda 表达式都属于纯右值。

   **将亡值(xvalue, expiring value)**，是 C++11 为了引入右值引用而提出的概念（因此在传统 C++中，纯右值和右值是统一个概念），也就是即将被销毁、却能够被移动的值。

   ```
   std::move
   ```




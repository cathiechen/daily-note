# google c++ code style guid

- head files
  - self-contained headers
    - 每个cc文件都要无条件地contain自己的head file
    - template和inline的声明和定义最好在同一个地方。若在不同文件，声明的地方应该include定义的地方。（把定义放到-inl.h里，已经out了）
    - 如果template的定义是private的，可以写到.cc文件中
    - 也有一些文件不是self-contained的，以.inc结尾
  - the #define guard
    - # define guard的作用是：防止重复包含
    - 格式：`<PROJECT>_<PATH>_<FILE>_H_`, path应该包含全路径.  


				#ifndef XXX_H_
				#define XXX_H_
					...
				#endif XXX_H_ //XXX_H_



	- forward declarations
		- 尽量别用
		- 定义：前置声明 `class xxx`, 而非`#include xxx_file.h`
		- pros:
			- 节省编译时间，因为include迫使编译器打开更多的文件，处理更多的输入，从而增加编译时间
			- 节省不必要的重复编译
		- cons：
			- 掩藏了依赖，导致错过必要的重编
			- 对后续修改造成障碍
			- std::的前置声明，会产生未定义的行为
			- include和前置声明语意上有点不同，在继承关系上面
			- 若是多个class，前置要多次，include一次就ok了...haha...
			- 构造代码用point member比object member更复杂更慢
		- 结论：
			- 尽量不用前置声明
			- 函数声明一定用include
			- 有template的class用include更好
	- inline function
		- 要求代码少于10行（反例:linebreaker里面的handletext）
		- 定义：inline function是expand到调用的地方，而非通过函数地址调用
		- 够短才效率，过长会让代码变慢
		- 结论：
			- 不超过10行，注意：析构函数中代码量往往比看起来的多
			- 一般有loop和switch的不用inline
			- 虚函数和递归调用的函数也不用inline
	- names and order of includes
		- 用相对于src的目录，不使用./ ../
		- 顺序：
			- self-contained，先包含自己的头文件
			- c system files
			- c++ system files
			- 其它库的.h文件
			- 项目中的.h文件
		- 按字母表顺序
		- 不要因为你引用的文件引用了你要引用的另一个文件，而不引用另一个文件。当然，除了自己的头文件
		- 跟系统相关的宏的条件引用，放到最后
- scope（作用域）
  - namespace
    - 把代码放到namespace里面，这样可以保证名字的唯一性
    - 不要用`using namespace xxx`
    - 不要用inline namespace
    - 定义：namespace把global scope划分成不同的区域，防止命名冲突
    - pros：名字变短了
    - 结论：
      - namespace的命名规则
        - 全小写，最上层的名字基于项目名
        - 避免重复
      - 结束的地方加注释
      - include，全局变量声明、定义，前置声明之后的所有代码放到namespace里面
      - 不要在std里声明任何东西
      - namespace的alliases不要放在头文件中
  - unnamed namespace and static variables
    - 什么是unnamed namespace？


				namespace {
					...
				}// namespace
	
		- 在其他文件用不到的.cc文件的定义，放到unnamedspace里面。或者用static声明。
		- internal linkage
		- 尽量在.cc文件中使用internal linkage，不在.h中使用
		- unnamed namespace结尾加注释
	
	- nonmember static member and global functions
		- nonmember function 放到namespce里面
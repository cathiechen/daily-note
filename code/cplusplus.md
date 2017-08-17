# C++

## override 和 final

- [override](http://zh.cppreference.com/w/cpp/language/override)：覆盖基类的虚函数

```
struct A
{
    virtual void foo();
    void bar();
};
 
struct B : A
{
    void foo() const override; // 错误： B::foo 不覆写 A::foo
                               // （签名不匹配）
    void foo() override; // OK ： B::foo 覆写 A::foo
    void bar() override; // 错误： A::bar 非虚
};
```

- [final](http://zh.cppreference.com/w/cpp/language/final): 
	- 指定某个虚函数不能被override
	- 指定某个类不能被继承
```
struct Base
{
    virtual void foo();
};
 
struct A : Base
{
    void foo() final; // A::foo 被覆写且是最终覆写
    void bar() final; // 错误：非虚函数不能被覆写或是 final
};
 
struct B final : A // struct B 为 final
{
    void foo() override; // 错误： foo 不能被覆写，因为它在 A 中是 final
};
 
struct C : B // 错误： B 为 final
{
};
``` 


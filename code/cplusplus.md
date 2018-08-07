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

## const, volatile, and mutable

- const，强调不可变。
- volatile，变量可能被程序以外的改变，编译器不进行相关优化，如：volation变量每次都从内存取，不从寄存器取。
- mutable，作用于成员变量，使该object是const的情况下，class内部可以改变mutable的值。

```
class HashTable {
 public:
    //...
    std::string lookup(const std::string& key) const
    {
        if (key == last_key_) {
            return last_value_;
        }

        std::string value{this->lookupInternal(key)};

        last_key_   = key;
        last_value_ = value;

        return value;
    }

 private:
    mutable std::string last_key_
    mutable std::string last_value_;
};
```



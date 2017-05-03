# js

一直想学js，但每次从if... else出发，感觉太无聊了。最近在网上发现一个大牛的[博客](http://www.ruanyifeng.com/blog/javascript/)，每篇都很短，知识点单一聚焦，很好！

### 探测本页面是否被frame，iframe包含。

这个挺必要的，其他人通过iframe包含你的页面，跟你自己的页面，肉眼上是分辨不出来的。这相对于页面的自我防卫。

- 页面端解决方法： 通过对比window.location.href 和 top.location.href。
- 浏览器端解决方法：可以给包含的页面发个event什么的（每次都发event，有没有创意？！但浏览器能做的，就是把当前页面的状态告诉页面啊...）

### js的闭包

js的函数是可以包行函数的，里层函数f2可以访问，外层函数f1的局部变量，还可以把里层的函数当返回值返回出去。这样，外层函数的外面就可以访问到外层函数的局部变量了。哈哈，外面世界要访问函数的局部变量呢？如果，返回的f2,被赋值与一个全局变量，在这个全局变量被释放前，f1里的局部变量都不会被释放。相当于是把局部变量全局化的一种方式。这样做的好处？可以任性。其它解决方法：把f1里的变量全局化。

例子：

	function f1() {
		var n = 111;
		function f2() {
			alert(n);
		}
		return f2;
	}

	var r = f1();
	r();

### 元素位置的获取方法

- clientHeight
- scrollHeight, scrollTop
- getBoundingClientRect

### js的封装
- [构造函数](http://www.ruanyifeng.com/blog/2010/05/object-oriented_javascript_encapsulation.html)
	- 基本封装
		var cat1 = {};
		cat1.name = "xxx";
		cat1.color = "xxx";
	- function 形式的封装
		function cat(name, color) {
			return { //js的‘{}’好神奇...
				name: name,
				color: color;
			}
		}
		var cat1 = cat("xxx", "xxx");
	- function + this的形式（多了两个属性： constructor, instanceof）
		function cat(name, color) {
			this.name = name;
			this.color = color;
		}
		var cat1 = new cat("xxx", "xxx");

	这样多了两个属性。`alert(cat2.constructor == Cat); //true` 和 `alert(cat1 instanceof Cat); //true`
	- prototype: 每个构造函数都有个prototype，可以把不变的变量放到这里。`alert(Cat.prototype.isPrototypeOf(cat1)); //true` `alert(cat1.hasOwnProperty("name")); // true` `alert("name" in cat1); // true` 


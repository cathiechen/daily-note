# will-change

## 简介

这是页面向浏览器预告接下来（很短时间）动作的方法，让浏览器提前做好准备。

## CSS的写法

这块基本上是参考[使用CSS3 will-change提高页面滚动、动画等渲染性能](https://www.w3cplus.com/css3/introduction-css-will-change-property.html)：

- 语法
 
		/* 关键字值 */
		will-change: auto;
		will-change: scroll-position;
		will-change: contents;
		will-change: transform;        /* <custom-ident>示例 */
		will-change: opacity;          /* <custom-ident>示例 */
		will-change: left, top;        /* 两个<animateable-feature>示例 */
		
		/* 全局值 */
		will-change: inherit;
		will-change: initial;
		will-change: unset;

- 使用
	- js

			dom.onmousedown = function() {
			    target.style.willChange = 'transform';
			};
			dom.onclick = function() {
			    // target动画哔哩哔哩...
			};
			target.onanimationend = function() {
			    // 动画结束回调，移除will-change
			    this.style.willChange = 'auto';
			};

	- CSS

			.will-change-parent:hover .will-change {
			  will-change: transform;
			}
			.will-change {
			  transition: transform 0.3s;
			}
			.will-change:hover {
			  transform: scale(1.5);
			}

	- 注意事项，使用是有代价的：
		- 使用过程中确保will-change预告的动作很快就会发生
		- 不要在正常形态下预告will-change
		- 使用完记得关闭

## blink的实现

- `SubtreeWillChangeContents()` `HasWillChangeCompositingHint()` ==> `kAllocateOwnCompositedLayerMapping`， 新建CompositedLayer
- `WillChangeScrollPosition` ==> '', blink里面好像还没有相关优化
- `has_will_change_transform_hint_` layer_impl.cc, 没看到用的地方
- `HasWillChangeTransformHint` ComputedStyle.h, 跟已经设置了transform一样处理，可能会先做些准备吧

		  // Return true if any transform related property (currently
		  // transform/motionPath, transformStyle3D, perspective, or
		  // will-change:transform) indicates that we are transforming.
		  // will-change:transform should result in the same rendering behavior as
		  // having a transform, including the creation of a containing block for fixed
		  // position descendants.
		  bool HasTransformRelatedProperty() const {
		    return HasTransform() || Preserves3D() || HasPerspective() ||
		           HasWillChangeTransformHint();
		  }
- `HasWillChangeOpacityHint()` ComputedStyle.h, 也一样，感觉像是浏览器内部的css trick

		  // Returns |true| if opacity should be considered to have non-initial value
		  // for the purpose of creating stacking contexts.
		  bool HasNonInitialOpacity() const {
		    return HasOpacity() || HasWillChangeOpacityHint() ||
		           HasCurrentOpacityAnimation();
		  }


## 参考

- [An Introduction to the CSS will-change Property](https://www.sitepoint.com/introduction-css-will-change-property/)
- [使用CSS3 will-change提高页面滚动、动画等渲染性能](https://www.w3cplus.com/css3/introduction-css-will-change-property.html)


#painter
paint改动比较大，以前老是看不懂，现在尝试看一下。
paint分不同的阶段，什么background、content、mask之类的。一层一层地画。还有graphiccontext，脏区域之类的问题。所以还是比较复杂。
现在paint变了，每个objec都有自己的painter，比如：基类objectpaint, blockpaint, tablepaint, tablecellpaint等等，这样结构比较简单一点。

##objectpaint
- 目录位于：src/third_party/WebKit/Source/core/paint/
- 与layout最近的一层paint。
- objectpaint以及它的子类，会在paint()中，把自己需要paint的各个阶段paint出来，并生成display item添加到下一次paint的displayitemlist中。displayitem的生成，可以是用drawrecord画，也可以是从currentPaintArtifact中获取可复用的display item
- 具体处理跟不同object的paint算法相关，如block，table等等

##displayItem
There will be one DisplayItem for each paint phase of each Render object. 

- 主要组成:
	- displayitemclient， 其实上是一个layoutobject
	- type
	- 是否可以缓存
- 作用
	- 每个layout object的每个阶段都有一个displayItem。这是blink往cc传递的最小单位。它的子类里面会包含着skpicture，一系列绘制指令。
- 创建
	- 通过drawrecord创建。drawrecord会创建一个displayItemList里面包含displayitem

##drawingDisplayItem
displayItem的一个子类。 displayItem结构其实很简单，没有保存指令的地方，指令应该是保存在子类中的

- 主要组成: skpicture
- 创建：在drawrecord析构的时候，会创建drawingDisplayItem

	`m_context.getPaintController().createAndAppend<DrawingDisplayItem>(...)`

##DrawingRecord
- 重要组成
	- graphiccontext
	- displayitemclient
	- displayitemtype
- 作用
	- 构造：gc.beginRecord
	- 析构：gc.restore, 往paintcontroller里面创建并添加displayItem

##paintController
这个类比较复杂，主要负责displayItem和paintchunk的管理，以及一些index，还有cache的一些流程

- 主要组成：
	- `PaintArtifact m_currentPaintArtifact;`最新的已完成的paintArtifact
	- `DisplayItemList m_newDisplayItemList;`上面DrawingRecord操作的list，组成新的paintArtifact的原料之一
	- `PaintChunker m_newPaintChunks;`组成新的paintArtifact的原料之一
	- `Vector<size_t> m_itemsMovedIntoNewList;`存储一系列的index，这个index对应的是`m_currentPaintArtifact.getDisplayItemList()`里面的序列，这index对应的displayItem需要被移动到m_newDisplayItemList中重复使用。
	- `IndicesByClientMap m_outOfOrderItemIndices;`这里面存的是可以被缓存的display item在`m_currentPaintArtifact.getDisplayItemList()`里面对应的index。
	- `IndicesByClientMap m_outOfOrderChunkIndices;` 这个跟上面那个应该差不多
	- `std::unique_ptr<RasterInvalidationTrackingMap<const PaintChunk>>      m_paintChunksRasterInvalidationTrackingMap;`
	- `using IndicesByClientMap = HashMap<const DisplayItemClient*, Vector<size_t>>;`
- `m_outOfOrderItemIndices`是如何维护的？
	- `findcachedispossible()`中，先在`m_outOfOrderItemIndices`中找display item，找到及返回index；找不到的话，遍历当前`m_currentPaintArtifact.getDisplayItemList()`，在找到display Item之前，把遇到的所有可以缓存的display item的index加到`m_outOfOrderItemIndices`中。
- 作用：主要是维护当前的displayitemlist，以及新的displayitemlist的生成。这其中会遇到缓存display item的逻辑：找到要重用的display item 在`m_currentPaintArtifact.getDisplayItemList()`中对应的index.

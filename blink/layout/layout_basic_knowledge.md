# layout 相关的基本知识

## container和containingBlock
- 为啥存在container 和 containingBlock：
  - 原因是：一般情况下，block是只containing block，所以有containingBlock。但有时layoutInline也有可能会成为包含块，所以有container。


## LayoutObject的继承关系

- LayoutObject
        - LayoutText
        - *LayoutBoxModel*
                - *LayoutInline*
                - *LayoutBox*
                        - LayoutReplace
                        - *LayoutListMarker*
                        - LayoutBlock
                                - *LayoutBlockFlow*
                                - flex
                                - grid
                                - table
                                - scrollBar

## node的继承关系

- EventTarget
  - Node
  - DocumentType
  - Attr
  - CharacterData(text)
  - ContainerNode
    - Document
    - DocumentFragment
    - Element
      - HTMLElement
        - ...
      - SVGElement
      - VTTElement
      - PseudoElement

## node和LayoutObject之间的联系

创建什么类型的object由CSS的display决定，具体见`LayoutObject::CreateObject`。也就是`display: block`创建layoutBlock， `display: inline`创建layoutInline

1. 为啥position会影响object的创建？

  原因是position影响display，详见：`StyleAdjuster::AdjustComputedStyle()`, `EquivalentBlockDisplay()`

1. display如果发生变化？
   因为display影响object的类型，所以发生变化是，将产生reattch的动作。详见：`ComputedStyle::StylePropagationDiff()`
   stylechange: NeedsReattachStyleChange的来由。在recalcOwnStyle发现需要reattach时，发起reattach，element先deattach，然后设置`setStyleChange(NeedsReattachStyleChange)`。然后attach，重新创建相应的object

```
>	minichrome.exe!blink::Node::setStyleChange(blink::StyleChangeType changeType) 行 809	C++
 	minichrome.exe!blink::Node::detach(const blink::Node::AttachContext & context) 行 1032	C++
 	minichrome.exe!blink::ContainerNode::detachChildren(const blink::Node::AttachContext & context) 行 319	C++
 	minichrome.exe!blink::ContainerNode::detach(const blink::Node::AttachContext & context) 行 876	C++
 	minichrome.exe!blink::Element::detach(const blink::Node::AttachContext & context) 行 1541	C++
 	minichrome.exe!blink::Node::reattach(const blink::Node::AttachContext & context) 行 1005	C++
 	minichrome.exe!blink::Element::recalcOwnStyle(blink::StyleRecalcChange change) 行 1712	C++
 	minichrome.exe!blink::Element::recalcStyle(blink::StyleRecalcChange change, blink::Text * nextTextSibling) 行 1656	C++



> 	minichrome.exe!blink::HTMLRubyElement::createLayoutObject(const blink::ComputedStyle & style) 行 27	C++
 	minichrome.exe!blink::LayoutTreeBuilderForElement::createLayoutObject() 行 120	C++
 	minichrome.exe!blink::LayoutTreeBuilderForElement::createLayoutObjectIfNeeded() 行 77	C++
 	minichrome.exe!blink::Element::attach(const blink::Node::AttachContext & context) 行 1477	C++
	minichrome.exe!blink::Node::reattach(const blink::Node::AttachContext & context) 行 1006	C++
```

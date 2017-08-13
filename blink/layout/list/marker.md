# 重构marker position的实现


## W3C中关于listMarker的规定

- 现有规定：
- 后续可能添加的规定：


## 现有方案

- `ListMarker`都是inline的
- `LayoutListItem::SubtreeDidChange()`时，调用`LayoutListItem::UpdateMarkerLocation()`;
  - 确定`line_box_parent`
    - 第一个line_box的父亲节点
    - `<li>`
  - remove marker from 之前的节点
  - `line_box_parent->AddChild(marker_, FirstNonMarkerChild(line_box_parent))`
  - `marker_->UpdateMarginsAndContent();`更新marker的content和margin(偏移设置在margin里面)
    - `LayoutListMarker`的layout
  - 除了计算宽高，还有计算一个值： `line_offset_`
  - `line_offset_`: `ListItem()->LogicalLeftOffsetForLine(block_offset, kDoNotIndentText, LayoutUnit());`= float object宽度 + `TextIndentOffset()`
- 在`LayoutBlockFlow::UpdateBlockLayout()`时，最后会`ComputeOverflow(unconstrained_client_after_edge);`
  - `AddOverflowFromChildren();`， 在`LayoutListItem::AddOverflowFromChildren()`还要`PositionListMarker();`
  - `PositionListMarker();`
    - 移动`marker_logical_left`, `marker_->InlineBoxWrapper()->MoveInInlineDirection()`, 里面包含`line_offset_`
    - 然后处理overflow的事情: 
      - marker的InlineFlowBox，以及其parent，OverrideOverflowFromLogicalRects, 重新计算的layoutrect和visionrect.
      - block level, ParentBox(), 向上传递overflow rect
- 总结：
  -  创建一个inline的marker
  - 把marker加上layout tree，并更新marker的margin和content
  - computeoverflow（li也就是layout完所有孩子后），重新更正marker的水平位置
  - 这个方案是：把marker加入content flow里，省略了垂直位置的的计算，只要计算水平位置，但存在以下问题
    - !overflow: visibility		
    - nested li
    - 空content高度
    - ...



## 新方案

- inside的marker作为inline； outside的marker作为absolute
  - absolute需要一个container，所以还是要（0,0,0,0）的relative container
  - 更新后不要管水平方向的位置变化，只要管垂直方向的，那line-height这个怎么算？
  - inside和outside互换的时候如何处理？
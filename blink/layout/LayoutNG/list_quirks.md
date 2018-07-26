# quirks of list

## fast/lists/list-item-line-height.html

上面测试了li不管有没有text，li的line-height都有效。
line-height是否有效，跟这个block是否存在text有关，若不存在text，line-height不会被应用。
为了实现这个，在创建inlineflowbox时，判断是否li，若是，则直接设置存在text：`has_text_children_`
详见：构造函数`InlineFlowBox(LineLayoutItem line_layout_item)`

LayoutNG中，`NGInlineLayoutAlgorithm::CreateLine`时，若当前node是list item且ListStyleType：none
`box->ComputeTextMetrics(line_style, baseline_type_);`
ComputeTextMetrics把style计算出来的metrics unite到box的metrics中，这样就可以影响行高了。

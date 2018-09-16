# FLOAT

float一直是个迷，不管是legacy layout还是LayoutNG。最近在处理list marker与float的关系，硬这头皮把float啃下来。list真是使人进步;)

## 什么时候layout float

由于margin collaps，float要在bfcoffset resolved之后，才能确定layout的位置。但bfcoffset有时候只能依靠后续的的节点才能被resolve。且float会影响到后续的排版宽度，所以float不能在所有节点都处理完之后再处理。
所以，在NG中，先把float加到`unpositioned_floats_`中，代码：`NGBlockLayoutAlgorithm::HandleFloat`。若bfcoffset resolved，处理列表里面的float，并把它们加到`positioned_floats`，代码：`NGBlockLayoutAlgorithm::PositionPendingFloats`。
对于float后面的节点，当bfcoffset没有Resolved时，在layout前期判断自己是否可以resolve bfcoffset，若可以，设置相关flag后，直接return。只有先处理完float后，后续节点的layout才有意义。

## float layout之后存在何处

在`ng_floats_utils.cc`中处理float，PositionFloat()。可以看到float在layout之后，生成了一个NGExclusion，并把这个NGExclusion加到`exclusion_space（NGExclusionSpace）`中。add的过程中，会生成shelf，并把shelf添加到shelves列表中。（这个过程好像很复杂，见：`NGExclusionSpace::Add`）
NGExclusion：代表一个float。包含的数据结构：NGBfcRect，EFloat，NGExclusionShapeData。 NGExclusionShapeData包含layoutobject，margin，`shape_insets`信息。

NGExclusionSpace：代表这个bfc中所有的NGExclusions，即float。这个类很重要啊，可以FindLayoutOpportunity()。
主要的数据结构有：NGExclusion列表，NGShelf列表，和NGLayoutOpportunity列表。
NGShelf：是一个架子，包含这个shelf起点的垂直位置：`block_offset`，最左最右位置：`line_left，line_right`，以及跟这个shelf相连的左右float列表：`line_left_edges; line_right_edges;`
NGLayoutOpportunity：最主要的是rect，记录这个opportunity的区域。
FindLayoutOpportunity()：提供`org_offset`和`available_size`以及`minimum_size`，这样它会找到一个适合的opportunity给你。`NGExclusionSpace::AllLayoutOpportunities`中，根据shelves和opportunities，找到所有符合要求的opp。

NGExclusionSpace 和 NGConstraintSpace的关系：错综复杂。。。

## float如何影响后续的其他节点
生成的NGExclusionSpace会在block中传给他们的孩子节点。
如果是inline节点，在`NGLayoutOpportunity::ComputeLineLayoutOpportunity`计算出`line_opportunity`，存在与lineinfo中，用于后续的inline排版。


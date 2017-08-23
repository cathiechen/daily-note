## Objective
This document aims to discuss the refactor list marker position. And this is the follow-up of the discuss in https://chromium-review.googlesource.com/c/605157.

## Current status

Currently list marker created as a inline replaced box. It would add to `<li>`'s `GetParentOfFirstLineBox` when the content of `<li>` isn't !overflow:visible, nested list, or have different text direction with `<li>`. Otherwise it will be add to `<li>`

This will make marker effect the height of `<li>` and generate unecessary line-break. E.g. 
1. The height of `li` with empty content: `<li></li>`.
2. Line break issues: `<li><div style='overflow: hidden'>text</div></li>`, there shouldn't be a line-break between marker and text.
3. Nested lists: `<li><ul><li>text</li></ul></li>`, there shouldn't be a line-break between marker and text.

According to [latest working draft](https://www.w3.org/TR/css-lists-3/#position-marker), markers counts as absolutely positioned. 

## Resolution 1

In this resolution, we make outside marker absolutely positioned and don't change the layout tree position of marker. Then adjust marker to right position based on its static position.

Changes required:
- `ListItemStyleDidChange()`: set `postion: absolute` to outside marker; set `position: relative` to inside marker.
- `LayoutListItem::UpdateMarkerLocation()`: add marker to `GetParentOfFirstLineBox`
- `LayoutListMarker::UpdateLayout()`: 
  - outside marker need to `ComputeInlineStaticDistance()` and `ComputeBlockStaticDistance()`, get the static position.
  - position marker:
    - inline direction: margin, float, indent...
    - block direction: position marker baseline flush against first line box baseline. Marker is a replaced box not a block, we couldn't position it by `line-height` or `vertical-align`.
- overflow rect: `LayoutListItem::PositionListMarker()` could be removed, because outside marker is absolute positioned and has adjusted inline direction.

## Resolution
#### Outside marker :

In this resolution we set outside marker absolutely position, and add outside marker as the child of `<li>`. As the container of marker is uncertain, we need to compute the static position of marker. As marker is the child of `<li>`, it could reduce position adjust in inline direction. As marker is absolutely positioned, add overflow rect from marker could be removed too.

#### Inside marker :

Make position of inside marker relative. Add inside marker to `GetParentOfFirstLineBox`, with no position adjust. If it's has different text direction to `<li>`, it should add as child of `<li>`. In this case , if `<li>` has block child, there will be a line-break between marker and content.

#### Here are the changes required:

- `ListItemStyleDidChange()`: Set `postion: absolute` to outside marker; set `position: relative` to inside marker. 
- `LayoutListItem::UpdateMarkerLocation()`: 
  - outside marker: Make outside marker as first child of `<li>`. 
  - Inside marker: Make inside marker as first child of `GetParentOfFirstLineBox`. If its text direction is different to `li`, add inside marker as first child of `<li>`. If an anonymous block is created as marker's parent, make this anonymous block inline.
  - `list-style-position` changed: If `list-style-position` changed, make sure marker removed from old position: `marker_->Remove()` and add it to the right position.
- `LayoutListMarker::UpdateLayout()`:
  - Outside marker:
    - Compute static position of marker.
    - Position marker baseline flush against `<li>` content's first line box baseline.
- overflow rect: `LayoutListItem::PositionListMarker()` could be removed, because outside marker is absolute positioned and adjusting inline direction position is no need.


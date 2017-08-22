## Objective
This document aims to discuss the refactor list marker. And its the follow-up of the discuss in https://chromium-review.googlesource.com/c/605157.

## Current status

Currently list marker created as a inline replaced box. It would add to `<li>`'s `GetParentOfFirstLineBox` when the content of `<li>` isn't !overflow:visible, nested list, or have different text direction with `<li>`. Otherwise it will be add to `<li>`

This will make marker effect the height of `<li>` and generate unecessary line-break. E.g. 
1. The height of `li` with empty content: `<li></li>`.
2. Line break issues: `<li><div style='overflow: hidden'>text</div></li>`, there shouldn't be a line-break between marker and text.
3. Nested lists: `<li><ul><li>text</li></ul></li>`, there shouldn't be a line-break between marker and text.

According to [latest working draft](https://www.w3.org/TR/css-lists-3/#position-marker), markers counts as absolutely positioned. 

## New Resolution

The new resolution make outside marker absolute positioned. And adjust marker to right position based on its static position.
- Changes:
  - `ListItemStyleDidChange()`: set `postion: absolute` to outside marker; set `position: relative` to inside marker.
  - `LayoutListItem::UpdateMarkerLocation()`: add marker to `GetParentOfFirstLineBox`
  - `LayoutListMarker::UpdateLayout()`: 
    - outside marker need to `ComputeInlineStaticDistance()` and `ComputeBlockStaticDistance()`, get the static position.
    - position marker:
      - inline direction: margin, float, indent...
      - block direction: position marker baseline flush against first line box baseline. Marker is a replaced box not a block, we couldn't position it by `line-height` or `vertical-align`.
  - overflow rect: `LayoutListItem::PositionListMarker()` could be removed, because outside marker is absolute positioned and has adjusted inline direct.

## Resolution 2
This resolution add outside marker into `<li>` instead of `<li>`'s `GetParentOfFirstLineBox`. This would reduce position adjust in inline director.

- changes require:
  - `ListItemStyleDidChange()`:`postion: absolute` to outside marker; set `position: relative` to inside marker. 
  - `LayoutListItem::UpdateMarkerLocation()`: make outside marker as a child of `<li>`, If `list-style-position` changed, make sure marker removed from old postion: `marker_->Remove()`
  - `LayoutListMarker::UpdateLayout()`:
    - compute static position of marker
    - position marker baseline flush against first line box baseline.
  - overflow rect: this could be removed too.



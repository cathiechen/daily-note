## Objective
This document aims to discuss the refactor list marker position. And this is the follow-up of discuss in https://chromium-review.googlesource.com/c/605157.

## Current status

Currently list marker created as an inline replaced box. It would add to `<li>`'s `GetParentOfFirstLineBox` when the content of `<li>` isn't !overflow:visible, nested list, or have different text direction with `<li>`. Otherwise, it will be added to `<li>`

This will make marker effect the height of `<li>` and generate unnecessary line-break. E.g. 
1. The height of `li` with empty content: `<li></li>`.
2. Line break issues: `<li><div style='overflow: hidden'>text</div></li>`, there shouldn't be a line-break between marker and text.
3. Nested lists: `<li><ul><li>text</li></ul></li>`, there shouldn't be a line-break between marker and text.

According to [the latest working draft](https://www.w3.org/TR/css-lists-3/#position-marker), markers counts as absolutely positioned. 

## Solution
#### Outside marker :

In this solution we set outside marker absolutely position, and add outside marker as the child of `<li>`. As the container of marker is uncertain, we need to compute the static position of marker. As marker is the child of `<li>`, it could reduce position adjust in inline direction. As marker is absolutely positioned, add overflow rect from marker could be removed too.

#### Inside marker :

Make position of inside marker relative. Add inside marker to `GetParentOfFirstLineBox`, with no position adjust. If it's has different text direction to `<li>`, it should add as child of `<li>`. In this case, if `<li>` has block child, there will be a line-break between marker and content.

#### Here are the changes required:

- `ListItemStyleDidChange()`: Set `position: absolute` to outside marker; set `position: relative` to inside marker. 
- `LayoutListItem::UpdateMarkerLocation()`: 
  - outside marker: Make outside marker as first child of `<li>`. 
  - Inside marker: Make inside marker as first child of `GetParentOfFirstLineBox`. If its text direction is different to `li`, add inside marker as first child of `<li>`. If an anonymous block is created as marker's parent, make this anonymous block inline.
  - `list-style-position` changed: If `list-style-position` changed, make sure marker removed from old position: `marker_->Remove()` and add it to the right position.
- `LayoutListMarker::UpdateLayout()`:
  - Outside marker:
    - Compute static position of marker.
    - Position marker baseline flush against `<li>` content's first line box baseline.
- Overflow rect: `LayoutListItem::PositionListMarker()` could be removed, because outside marker is absolute positioned and adjusting inline direction position is no need.


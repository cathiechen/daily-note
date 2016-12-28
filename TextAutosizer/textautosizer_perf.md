#Analysis of performance [issue 675534](https://bugs.chromium.org/p/chromium/issues/detail?id=675534).

[flexbox-lots-of-data.html](https://chromium.googlesource.com/chromium/src/+/master/third_party/WebKit/PerformanceTests/Layout/flexbox-lots-of-data.html) is used to test the performance of layout. Using js code below to trigger layout of whole <body>.

	run: function() {
            document.body.style.width = ++index % 2 ? "99%" : "98%";
            document.body.offsetHeight;
        }

It's brilliant!


##Reason for this performance isssue:

According to the logic of flexbox layout.

case 1:

	<div id="flexbox" style="display:flex">
		<div id="item1">one-line text</div>
		<div id="item2">one-line text</div>
		<div id="item3">one-line text</div>
	</div>
When layout comes to "flexbox", layoutFlexItems() will layout its children in this order: item1, item2, item3.

case 2:

	<div id="flexbox" style="display:flex">
		<div id="item1">one-line text</div>
		<div id="item2">one-line text</div>
		<div id="item3">multi-line text, lots text lots text lots text...</div>
	</div>
Case 2, layoutFlexItems() will layout children in this order: item1, item2, item3, item1, item2. **2 times more**. LayoutFlexibleBox::applyStretchAlignmentToChild will trigger the last two layout.

Back to [flexbox-lots-of-data.html](https://chromium.googlesource.com/chromium/src/+/master/third_party/WebKit/PerformanceTests/Layout/flexbox-lots-of-data.html). There are many flexboxs like case 1. After autosize these flexboxs will become case 2. Cause the enlarging of text font size will make "one-line text" to "multi-line text". And it'll trigger 600+ times layout more than non autosize. So that's why this CL cause the performance issue.

##Difference between before and after this CL

	----------------------------------------------------------------------
	          | first layout pass | second and subsequent layout pass 
	----------------------------------------------------------------------
	before CL | autosize item3    | don't autosize item3              
	----------------------------------------------------------------------
	after CL  | autosize item3    | autosize item3             
	----------------------------------------------------------------------

- first layout pass:
	- As we can see, at the first line of [flexbox-lots-of-data.html](https://chromium.googlesource.com/chromium/src/+/master/third_party/WebKit/PerformanceTests/Layout/flexbox-lots-of-data.html),  item1 and item2 are empty. So when the supercluster of flexboxs tying to find a widthprovider, it will be item3. And there are enough text to autosize, so that's why the first layout pass will always autosize item3.
-  second and subsequent layout pass:
	- before CL: **recalculate the supercluster's multiplier**, and found a right widthprovider that's flexbox itself. At this time, the text is not enough, so the supercluster's multiplier will update to 1.
	- after CL: **reuse the old supercluster's multiplier from the first layout pass**, won't recalculate multiplier at all.


##Solution

Reusing the old supercluster's multiplier seems to make it more consistent.

In order to make flexbox-lots-of-data.html be comparable with before. Maybe we could make a slightly change  to [flexbox-lots-of-data.html](https://chromium.googlesource.com/chromium/src/+/master/third_party/WebKit/PerformanceTests/Layout/flexbox-lots-of-data.html). Make the item1 and item2 of the first flexbox non empty.


##The apparence of flexbox-lots-of-data.html

before CL:

![before](http://i.imgur.com/dLMTOuZ.png) 

after CL:

![after](http://i.imgur.com/evpDRKf.png)

 

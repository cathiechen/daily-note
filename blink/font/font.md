#font相关

画了一整天的流程图，尽量把它记下来吧，不然过几天又忘了。其实忘得差不多了！

##fontcache
最重要的应该是fontcache。font信息的存储方式。

- fontCache(global)
	- gFontDataCache
		- fontMetrics, widths
		- customFontData
			- fontface(可能是字的样子)
		- fontPlatformData
	- gfontPlatformDataCache
		- textsize
		- orientation
		- halfbuzzface(这个应该是通过id获取字体的类)
			- fontPlatformData
			- uniqueID
			- halfbuzzFontData
				- 从halfbuzzFontCache中获取出来的
	- gfallbackListShapeCache(ShapeCache)(跟形状相关，以textrun为单位，width经常会用到)
		- hashmap
			- key：fontDescription(里面有family！)
			- value：ShapeCache
				- singleCharMap
					- key: textRun[0]
					- value: shapeCacheEntry(shapeResult)
						- width
						- glyphBoundingBox(glyphoverflow和这个相关)
						- runs
						- simpleFontData
				- smallStringMap
					- key与singleCharMap有些不同，其它都一样。
					- key： 通过特定方法将string上的内容和其他信息，生成一个hash，这个hash作为key


##如何使用fontcache?

- fontSelector(cssFontCacheClient)
	- GenericFontFamilySettings
	- FontFaceCache(FontFace)
		- family
		- font style (italy)
		- font weight
		- cssFontFace
		- [fontface](https://www.w3cplus.com/content/css3-font-face)的css写法
		

				   @font-face {
				      font-family: 'SingleMaltaRegular';
				      src: url('../fonts/singlemalta-webfont.eot');
				      src: url('../fonts/singlemalta-webfont.eot?#iefix') format('embedded-opentype'),
				           url('../fonts/singlemalta-webfont.woff') format('woff'),
					   url('../fonts/singlemalta-webfont.ttf') format('truetype'),
					   url('../fonts/singlemalta-webfont.svg#SingleMaltaRegular') format('svg');
				      font-weight: normal;
				      font-style: normal;
				   }

	- styleEngine(CSSFontSelectorClient): 跟document一起初始化，每个document一个styleEngine
	- 作用：css <--->selector <----->fontfacecache, 当页面通过css改变fontfacecache时（add，remove），version++。 version的作用是，判断fontcache里面的保存的东西，是不是最新版本的。 
	- 比如：
		- 页面设置fontface
		- selector的version++
		- layout的时候，来获取文字的宽度
		- 到font.style.width，font里面有个fontfallbacklist可以去到shapecache
		- 获取fontcache里的shapecache，version不一样，清空shapecache
		- 根据run去获取shaperesult，空的，新建一个，并加入shapecache
		- 如何新建shaperesult，利用halfbuzz的接口，具体见` HarfBuzzShaper::shape`
	- 如果fontcache不是空的，且命中，流程是这样的
		- layout的时候，来获取文字的宽度
		- 到font.style.width，font里面有个fontfallbacklist可以去到shapecache
		- 获取fontcache里的shapecache，version一样，不清空shapecache
		- 根据run去获取shaperesult，找到shaperesult，获取宽度，返回

- font
	- fontdescription
	- FontFallbackList，集合shapecache，fontselector，fontdata等数据

- `LayoutText::computePreferredLogicalWidths`里面的` HashSet<const SimpleFontData*> fallbackFonts;`的作用
	- `fallbackFonts`如何被赋值的？

			void ShapeResult::fallbackFonts(
			    HashSet<const SimpleFontData*>* fallback) const {
			  ASSERT(fallback);
			  ASSERT(m_primaryFont);
			  for (unsigned i = 0; i < m_runs.size(); ++i) {
			    if (m_runs[i] && m_runs[i]->m_fontData &&
			        m_runs[i]->m_fontData != m_primaryFont &&
			        !m_runs[i]->m_fontData->isTextOrientationFallbackOf(
			            m_primaryFont.get())) {
			      fallback->insert(m_runs[i]->m_fontData.get());
			    }
			  }
			}

	- 使用到的地方
	
			  // We shouldn't change our mind once we "know".
			  ASSERT(!m_knownToHaveNoOverflowAndNoFallbackFonts ||
			         (fallbackFonts.isEmpty() && glyphOverflow.isApproximatelyZero()));
			  m_knownToHaveNoOverflowAndNoFallbackFonts =
			      fallbackFonts.isEmpty() && glyphOverflow.isApproximatelyZero();

	- 意思是，如果css或文字没有变化（见`m_knownToHaveNoOverflowAndNoFallbackFonts`的赋值），这个layouttext的`SimpleFontData`应该保持一致要么一样要么不一样不变。
	


# [Shaking Issue](https://codereview.chromium.org/2817443003/)性能问题

## select-long-word

- 测试内容：测试`sel.modify("extend", "forward", "word")`
- 流程：modify的时间主要花在找下一个word上。寻找的算法是这样的：遍历当前node的textinlinebox， 查找这些textinlinebox中是否存在可break的字符。最后，返回position。
- 分析：字体变大，导致textinlinebox变多，最后造成性能下降的假象。

- 

## multicol_lots-of-text-balanced

- 测试内容：测试multicolumn的balance效率
- 现象： 原本不放大的字体被放大了
- 解决方法： multicolumn 需要自己独立成一个cluster，并且在计算宽度的时候应该考虑到分列的情况
- 犹豫： 明知道它里面的内容是一致的，但它一个column的宽度很小，我们需要用哪个宽度？container or column？

## html5-full-layout

- 一部分由muli-columns引起
- 一部分由于字体放大引起： 由于页面内容很多，无法具体调查哪里性能有问题。通过对autosize打log，加和未加上shaking的修改对比：加（其中一个cluster以宽度600px计算multiplier）；未加(那个cluster的宽度是448px) 加上之前的经验，推断可能是字体放大导致一些功能更加耗时。为了验证这个想法，分别做下面4组实验：去除shaking修改（cluster 448）； 去除shaking修改，强制改cluster宽度600; 加上shaking修改(cluster 600);加上shaking修改，强制改cluster宽度448。
- 通过数据对比，可以得出性能变化只能字体大小有关。（autosize没有新增耗时）



In order to prevent the shaking issue, this patch make root cluster use LayoutView as WidthProvider to cluster multiplier. It might enlarge the font size  a bit.

select-long-word:
In this test case root cluster's WidthProvider is 800px(<div id="long">). After this patch it changed to 980px(LayoutView). So the font size of content changed from 40px to 49px.

'select-long-word' tests the performance of `sel.modify("extend", "forward", "word")`. Finding the end position of word cost the most part of time. The algorithm of 







Reason of regression:
Test case: multicol_lots-of-text-balanced tests the performance of multi-columns balance. During balancing columns height, need to traverse each line of content. If font-size enlarged and more line box generated, the performance would go down.

Why font-size enlarged?
Here is the test case:
<html>
  <body>
    <div style="-webkit-columns:3; orphans:2; widows:2;">
      lots of text......
    </div>
  </body>
</html>

There is only one cluster created by LayoutView in this page. <div> will use the multiplier of LayoutView cluster. After last CL(2817443003: font-size shaking), LayoutView cluster will use itself as width provider, so <div> will be autosized.

Solution:
The layout width difference between multi-columns and its anonymous child is large (anonymous-child-width * column-count = multi-columns-width). multi-columns elements should create its own cluster and calculate own multiplier.















Start Tracing Request: {'params': {'transferMode': 'ReturnAsStream', 'options': 'record-as-much-as-possible', 'categories': 'benchmark,blink.console,toplevel,trace_event_overhead,webkit.console'}, 'method': 'Tracing.start'}
(INFO) 2017-07-03 19:57:39,403 chrome_tracing_agent.RecordClockSyncMarker:182  Chrome version: 0
(INFO) 2017-07-03 19:57:44,987 trace_data.Serialize:189  Trace sizes in bytes: {'traceEvents': 6399076, 'telemetry': 31577, 'tabIds': 40}



  RunBenchmark at /big/newMyChromium/src/third_party/catapult/telemetry/telemetry/internal/story_runner.py:398
    expectations=expectations, metadata=benchmark.GetMetadata())
  Run at /big/newMyChromium/src/third_party/catapult/telemetry/telemetry/internal/story_runner.py:262
    _RunStoryAndProcessErrorIfNeeded(story, results, state, test)
  _RunStoryAndProcessErrorIfNeeded at /big/newMyChromium/src/third_party/catapult/telemetry/telemetry/internal/story_runner.py:99
    state.RunStory(results)
  traced_function at /big/newMyChromium/src/third_party/catapult/common/py_trace_event/py_trace_event/trace_event_impl/decorators.py:52
    return func(*args, **kwargs)
  RunStory at /big/newMyChromium/src/third_party/catapult/telemetry/telemetry/page/shared_page_state.py:296
    self._current_page.Run(self)
  Run at /big/newMyChromium/src/third_party/catapult/telemetry/telemetry/page/__init__.py:112
    self.RunPageInteractions(action_runner)
  RunPageInteractions at /big/newMyChromium/src/tools/perf/page_sets/tough_scrolling_cases.py:25
    synthetic_gesture_source=self.synthetic_gesture_source)
  traced_function at /big/newMyChromium/src/third_party/catapult/common/py_trace_event/py_trace_event/trace_event_impl/decorators.py:75
    return func(*args, **kwargs)
  ScrollPage at /big/newMyChromium/src/third_party/catapult/telemetry/telemetry/internal/actions/action_runner.py:435
    use_touch=use_touch, synthetic_gesture_source=synthetic_gesture_source))
  traced_function at /big/newMyChromium/src/third_party/catapult/common/py_trace_event/py_trace_event/trace_event_impl/decorators.py:75
    return func(*args, **kwargs)
  _RunAction at /big/newMyChromium/src/third_party/catapult/telemetry/telemetry/internal/actions/action_runner.py:56
    action.RunAction(self._tab)
  traced_function at /big/newMyChromium/src/third_party/catapult/common/py_trace_event/py_trace_event/trace_event_impl/decorators.py:75
    return func(*args, **kwargs)
  RunAction at /big/newMyChromium/src/third_party/catapult/telemetry/telemetry/internal/actions/scroll.py:108
    logging.info('---------------cc code %s', code)
NameError: global name 'logging' is not defined




  <module> at /big/newMyChromium/src/tools/perf/run_benchmark:26
    sys.exit(main())
  main at /big/newMyChromium/src/tools/perf/run_benchmark:22
    return benchmark_runner.main(config, [trybot_command.Trybot])
  main at /big/newMyChromium/src/third_party/catapult/telemetry/telemetry/benchmark_runner.py:440
    return command_instance.Run(options)
  Run at /big/newMyChromium/src/third_party/catapult/telemetry/telemetry/benchmark_runner.py:245
    return min(255, self._benchmark().Run(args))
  Run at /big/newMyChromium/src/third_party/catapult/telemetry/telemetry/benchmark.py:94
    return story_runner.RunBenchmark(self, finder_options)
  RunBenchmark at /big/newMyChromium/src/third_party/catapult/telemetry/telemetry/internal/story_runner.py:421
    results.PrintSummary()
  PrintSummary at /big/newMyChromium/src/third_party/catapult/telemetry/telemetry/internal/results/page_test_results.py:366
    output_formatter.Format(self)
  Format at /big/newMyChromium/src/third_party/catapult/telemetry/telemetry/internal/results/html_output_formatter.py:54
    histograms = self._ConvertChartJson(page_test_results)
  _ConvertChartJson at /big/newMyChromium/src/third_party/catapult/telemetry/telemetry/internal/results/html_output_formatter.py:32
    page_test_results.all_summary_values)
  ResultsAsChartDict at /big/newMyChromium/src/third_party/catapult/telemetry/telemetry/internal/results/chart_json_output_formatter.py:75
    logging.info('---------------result %s', result_dict)










In order to prevent the shaking issue, this patch make root cluster use LayoutView as WidthProvider to cluster multiplier. It might enlarge the font size  a bit.

select-long-word:
In this test case root cluster's WidthProvider is 800px(<div id="long">). After this patch it changed to 980px(LayoutView). So the font size of content changed from 40px to 49px.

'select-long-word' tests the performance of `sel.modify("extend", "forward", "word")`. Finding the end position of word cost the most part of time. The algorithm of








Sorry for the delay.

Here is the summary of this issues.

In order to prevent the shaking issue, this CL make root cluster use LayoutView as WidthProvider that might enlarge font size a bit on some page.

1. Issue 731681: multicol_lots-of-text-balanced, fixed. For more details https://codereview.chromium.org/2961583003

2. Issue 731659: select-long-word and Issue 732309: tough_scrolling_cases, won't fix. Because this regression is caused by font size enlarging.

  2.1 select-long-word:
    In this case before this CL root cluster's WidthProvider is 800px(<div id="long">). After this CL it's changed to 980px(LayoutView). So the font size multiplier change from 2.5 to 3.0625.
    'select-long-word' tests the performance of `getSelection().modify("extend", "forward", "word")`. Finding the end position of word cost the most time.
    According to the procedure of VisibleUnits.cpp NextBoundaryAlgorithm(), it will "Keep asking the iterator for chunks until the search function returns an end value not equal to the length of the string passed to it." These chunks is generated by inlineTextBox. As the font size enlarged, it would generate more inlineTextBox. That's the reason of this regression.

  2.2 tough_scrolling_cases:
    Before this CL root cluster's WidthProvider is 964px(<body>), after WidthProvider is 980px(LayoutView). So multiplier change from 3.0125 to 3.0625.
    According to [scroll.js](https://cs.chromium.org/chromium/src/third_party/catapult/telemetry/telemetry/internal/actions/scroll.js)  ScrollAction.prototype.startGesture_ 
        var max_scroll_length_pixels = (MAX_SCROLL_LENGTH_TIME_MS / 1000) * this.options_.speed_;
        var distance = Math.min(max_scroll_length_pixels, this.getScrollDistance_());
    The scroll `distance` might be `this.getScrollDistance_()` if speed_ is big enough.
        ScrollAction.prototype.getScrollDistanceDown_ = function() {
          var clientHeight;
          // clientHeight is "special" for the body element.
          if (this.element_ == document.body)
            clientHeight = __GestureCommon_GetWindowHeight();
          else
            clientHeight = this.element_.clientHeight;
          return this.element_.scrollHeight -
                 this.element_.scrollTop -
                 clientHeight;
        }; 
    `this.element` is <HTML>. For the font size enlarged, the distance will bigger than before. That's the reason of regression.

3. Issue 732311: html5-full-render, couldn't reproduce.
  Couldn't reproduce this regression at my local nexus7 telemetry environment. alexclarke@ could you check it again? 







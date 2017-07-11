# Telemetry

提了个字体放大的patch，引起了好几个性能预警。走了一遍chrome的性能测试：）人生无处不惊喜，咩哈哈。。


把遇到的各种坑记录下来，不然又忘记了。

## 测试用例们

有些测试用例不需要在telemetry跑也可以看到结果(dom)，但有些就必需(tough scroll). telemetry的一个功能就是记录之前的测试结果。

- 不需要的测试用例们：测试逻辑之前好像也有写过，大概就是一遍就是一个runner，给每个runner计时，一般单独跑页面是20次，然后把20次结果输出到页面中。

- 需要测试的用例们：其实就是scroll，scroll一次可以输出多个测试结果。具体怎么回事没看懂，应该是trace的某些数据，分解成不同的测试标准。大概逻辑应该是这样的：

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


最终逻辑在scroll.js中，通过scroll.py往js中传参数，进而控制scroll的页面及速度。


## 不同手机的测试结果要分开

一开始用s4跑html5-full-render，差异巨大，而且巨慢。 然后搭好了n7的环境。n7可以在s4的后面，基本上就是直线，以至于我觉得bug没法重现。最后才知道自己傻逼了。n
7也是有差异的，只是跟s4比太小了，所以在统一张图中显示不出来。。

## 尽量用脚本帮你干活

人干活有个莫非定律，能错的就对不了。所以才有把with和without patch的包弄反的事情发生。脚本可以帮你省很多时间和精力。以后，但凡有重复的工作，都应该首先想想可否脚本？

比如（这时候不装b，啥时候装？！）：
1. 编译的时候经常要很长时间，而且每次要等编译完安装apk，但每次都走神，一直敲命令也很烦。所以，就有了cc_update。人生简直美好太多了！
2. 各种测试命令一大堆，参数什么的又记不住，每次都要翻笔记！于是有了cc_unitest_autosize, cc_layouttest, cc_log等
3. 当然最想讲的是cc_run。先从cc_telemetry讲起。一开始只是run_benchmark，但后来就想着传个跑的次数进去。然后，两个不同的包切换，每次改脚本很麻烦，于是又多了一个指定apk的参数。再后来切换到n7后，安装apk太耗时，没法忍受，故有了先卸载，后安装，在跑测试。再后来又加了测试结果清空进去。然后觉得每次对比都要敲两次命令，烦！故有了cc_run. 两次一起跑，并可以指定次数。其实，后来还觉得经常切换网络麻烦，应该把切换工作加到脚本内。


cc_run代表了脚本形成的过程，不是一开始想设计成怎样，而是根据需求变化。但，如果后面写多了，自然会有好的设计在里面。设计是应该随时注意的事情。

## 当然还要get各种root技能

发现自己容易做些没用的事情，把时间耗费在上面，而不是思考，或查资料。这个应该改！

最终还是花很长时间纠结，而不是花时间在想办法解决上。。。

## 总结也有问题

后面的bug总结也不好，感觉把问题丢给其它的人嫌疑。而且时间很多花在环境上，重现测试结果上，汇报也不及时，应该改。

记得有人说过，如果工作你觉得做得很困难，那你就在成长。嗯，有道理！ 加油，saipan回来再战！


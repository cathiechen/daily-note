# useful stack

```
#0  0x00007ffff52cb775 in content::InProcessRendererThread::InProcessRendererThread(content::InProcessChildThreadParams const&) (this=0x244ebb952560, params=...)
    at ../../content/renderer/in_process_renderer_thread.cc:24
#1  0x00007ffff52cb98b in content::CreateInProcessRendererThread(content::InProcessChildThreadParams const&) (params=...) at ../../content/renderer/in_process_renderer_thread.cc:77
#2  0x00007ffff493d359 in content::RenderProcessHostImpl::Init() (this=0x244ebba41020) at ../../content/browser/renderer_host/render_process_host_impl.cc:859
#3  0x00007ffff442e7aa in content::RenderFrameHostManager::InitRenderView(content::RenderViewHostImpl*, content::RenderFrameProxyHost*) (this=0x244ebb716db0, render_view_host=0x244ebb934c80, proxy=0x0)
    at ../../content/browser/frame_host/render_frame_host_manager.cc:1920
#4  0x00007ffff4425b6d in content::RenderFrameHostManager::ReinitializeRenderFrame(content::RenderFrameHostImpl*) (this=0x244ebb716db0, render_frame_host=0x244ebb501020)
    at ../../content/browser/frame_host/render_frame_host_manager.cc:2097
#5  0x00007ffff44247a3 in content::RenderFrameHostManager::Navigate(GURL const&, content::FrameNavigationEntry const&, content::NavigationEntryImpl const&, bool) (this=0x244ebb716db0, dest_url=..., 
    frame_entry=..., entry=..., is_reload=false) at ../../content/browser/frame_host/render_frame_host_manager.cc:231
#6  0x00007ffff43d2223 in content::NavigatorImpl::NavigateToEntry(content::FrameTreeNode*, content::FrameNavigationEntry const&, content::NavigationEntryImpl const&, content::ReloadType, bool, bool, bool, scoped_refptr<content::ResourceRequestBodyImpl> const&) (this=
    0x244ebb5e73a0, frame_tree_node=0x244ebb716da0, frame_entry=..., entry=..., reload_type=content::ReloadType::NONE, is_same_document_history_load=false, is_history_navigation_in_new_child=false, is_pending_entry=true, post_body=...) at ../../content/browser/frame_host/navigator_impl.cc:373
#7  0x00007ffff43d3468 in content::NavigatorImpl::NavigateToPendingEntry(content::FrameTreeNode*, content::FrameNavigationEntry const&, content::ReloadType, bool) (this=0x244ebb5e73a0, frame_tree_node=0x244ebb716da0, frame_entry=..., reload_type=content::ReloadType::NONE, is_same_document_history_load=false) at ../../content/browser/frame_host/navigator_impl.cc:467
#8  0x00007ffff43a497a in content::NavigationControllerImpl::NavigateToPendingEntryInternal(content::ReloadType) (this=0x244ebb193d40, reload_type=content::ReloadType::NONE)
    at ../../content/browser/frame_host/navigation_controller_impl.cc:1966
#9  0x00007ffff439cb97 in content::NavigationControllerImpl::NavigateToPendingEntry(content::ReloadType) (this=0x244ebb193d40, reload_type=content::ReloadType::NONE)
    at ../../content/browser/frame_host/navigation_controller_impl.cc:1908
#10 0x00007ffff439cfe5 in content::NavigationControllerImpl::LoadEntry(std::unique_ptr<content::NavigationEntryImpl, std::default_delete<content::NavigationEntryImpl> >) (this=0x244ebb193d40, entry=...)
    at ../../content/browser/frame_host/navigation_controller_impl.cc:458
#11 0x00007ffff439ec95 in content::NavigationControllerImpl::LoadURLWithParams(content::NavigationController::LoadURLParams const&) (this=0x244ebb193d40, params=...)
    at ../../content/browser/frame_host/navigation_controller_impl.cc:788
#12 0x0000000000a4b8eb in content::Shell::LoadURLForFrame(GURL const&, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&) (this=
    0x244ebb96ee30, url=..., frame_name="") at ../../content/shell/browser/shell.cc:200
#13 0x0000000000a4b7fe in content::Shell::LoadURL(GURL const&) (this=0x244ebb96ee30, url=...) at ../../content/shell/browser/shell.cc:192
#14 0x0000000000a4b76b in content::Shell::CreateNewWindow(content::BrowserContext*, GURL const&, scoped_refptr<content::SiteInstance> const&, gfx::Size const&) (browser_context=
    0x244ebb6c4c20, url=..., site_instance=..., initial_size=...) at ../../content/shell/browser/shell.cc:187
#15 0x0000000000aa7e90 in content::ShellBrowserMainParts::InitializeMessageLoopContext() (this=0x244ebb4d99a0) at ../../content/shell/browser/shell_browser_main_parts.cc:162
#16 0x0000000000aa8221 in content::ShellBrowserMainParts::PreMainMessageLoopRun() (this=0x244ebb4d99a0) at ../../content/shell/browser/shell_browser_main_parts.cc:193
#17 0x00007ffff4095261 in content::BrowserMainLoop::PreMainMessageLoopRun() (this=0x244ebb1e6c20) at ../../content/browser/browser_main_loop.cc:1165
#18 0x00007ffff35d4295 in base::internal::FunctorTraits<void (content::IndexedDBCallbacksImpl::InternalState::*)(), void>::Invoke<content::IndexedDBCallbacksImpl::InternalState*>(void (content::IndexedDBCallbacksImpl::InternalState::*)(), content::IndexedDBCallbacksImpl::InternalState*&&) (method=(void (content::IndexedDBCallbacksImpl::InternalState::*)(content::IndexedDBCallbacksImpl::InternalState * const)) 0x7ffff4095140 <content::BrowserMainLoop::PreMainMessageLoopRun()>, receiver_ptr=@0x7fffffffcf90: 0x244ebb1e6c20) at ../../base/bind_internal.h:214
#19 0x00007ffff40a0761 in base::internal::InvokeHelper<false, int>::MakeItSo<int (content::BrowserMainLoop::* const&)(), content::BrowserMainLoop*>(int (content::BrowserMainLoop::* const&)(), content::BrowserMainLoop*&&) (functor=@0x244ebb553c10: (int (content::BrowserMainLoop::*)(content::BrowserMainLoop * const)) 0x7ffff4095140 <content::BrowserMainLoop::PreMainMessageLoopRun()>, args=@0x7fffffffcf90: 0x244ebb1e6c20) at ../../base/bind_internal.h:285
#20 0x00007ffff40a0707 in base::internal::Invoker<base::internal::BindState<int (content::BrowserMainLoop::*)(), base::internal::UnretainedWrapper<content::BrowserMainLoop> >, int ()>::RunImpl<int (content::BrowserMainLoop::* const&)(), std::tuple<base::internal::UnretainedWrapper<content::BrowserMainLoop> > const&, 0ul>(int (content::BrowserMainLoop::* const&)(), std::tuple<base::internal::UnretainedWrapper<content::BrowserMainLoop> > const&, base::IndexSequence<0ul>) (functor=@0x244ebb553c10: (int (content::BrowserMainLoop::*)(content::BrowserMainLoop * const)) 0x7ffff4095140 <content::BrowserMainLoop::PreMainMessageLoopRun()>, bound=std::tuple containing = {...}) at ../../base/bind_internal.h:361
#21 0x00007ffff40a064c in base::internal::Invoker<base::internal::BindState<int (content::BrowserMainLoop::*)(), base::internal::UnretainedWrapper<content::BrowserMainLoop> >, int ()>::Run(base::internal::BindStateBase*) (base=0x244ebb553bf0) at ../../base/bind_internal.h:339
#22 0x00007ffff34bda9b in base::internal::RunMixin<base::Callback<void (), (base::internal::CopyMode)1, (base::internal::RepeatMode)1> >::Run() const (this=0x244ebb157f80) at ../../base/callback.h:85
#23 0x00007ffff4c1e80b in content::StartupTaskRunner::RunAllTasksNow() (this=0x244ebb5548a0) at ../../content/browser/startup_task_runner.cc:45
#24 0x00007ffff4092e80 in content::BrowserMainLoop::CreateStartupTasks() (this=0x244ebb1e6c20) at ../../content/browser/browser_main_loop.cc:973
#25 0x00007ffff40a3efd in content::BrowserMainRunnerImpl::Initialize(content::MainFunctionParams const&) (this=0x244ebb200a40, parameters=...) at ../../content/browser/browser_main_runner.cc:126
#26 0x0000000000a509c4 in ShellBrowserMain(content::MainFunctionParams const&, std::unique_ptr<content::BrowserMainRunner, std::default_delete<content::BrowserMainRunner> > const&) (parameters=..., main_runner=Python Exception <class 'TypeError'> expected string or bytes-like object: 
...) at ../../content/shell/browser/shell_browser_main.cc:23
#27 0x0000000000a2a785 in content::ShellMainDelegate::RunProcess(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, content::MainFunctionParams const&) (this=
    0x7fffffffdd38, process_type="", main_function_params=...) at ../../content/shell/app/shell_main_delegate.cc:294
---Type <return> to continue, or q <return> to quit---
#28 0x00007ffff588d35b in content::RunNamedProcessTypeMain(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, content::MainFunctionParams const&, content::ContentMainDelegate*) (process_type="", main_function_params=..., delegate=0x7fffffffdd38) at ../../content/app/content_main_runner.cc:403
#29 0x00007ffff588f70c in content::ContentMainRunnerImpl::Run() (this=0x244ebb171380) at ../../content/app/content_main_runner.cc:793
#30 0x00007ffff588c8a2 in content::ContentMain(content::ContentMainParams const&) (params=...) at ../../content/app/content_main.cc:20
#31 0x00000000004a1d71 in main(int, char const**) (argc=4, argv=0x7fffffffde88) at ../../content/shell/app/shell_main.cc:48


```

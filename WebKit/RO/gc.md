# The GC issue

## What's the GC issue.

Basically, it's the JSElement that RO refer to got GCed when the node is removed from DOM tree.

Then, if RO use this JSElement again, it will create a new one. The attribute in the old one became undefined now.

There are two ways to lose JSElements. 

## ResizeObserver lose JSElement

```
<!DOCTYPE html><!-- webkit-test-runner [ experimental:ResizeObserverEnabled=true ] -->
<html>
<body>
<p>This tests that JS wrappers of targets removed from document to be delivered to an resize observer do not get collected.</p>
<pre id="log"></pre>
<script src="../resources/gc.js"></script>
<script>

if (window.testRunner)
    testRunner.dumpAsText();

const targetCount = 5;
const iterationCount = 10;
var deadCount = 0;

async function runAll() {
    if (window.testRunner)
        testRunner.waitUntilDone();

    for (let i = 0; i < iterationCount; ++i) {
        runTest();
        gc();
        await new Promise((resolve) => requestAnimationFrame(resolve))
    }

    document.getElementById('log').textContent = (deadCount ? `FAIL - ${deadCount} targets lost JS wrappers` : 'PASS') + '\n';

    if (window.testRunner)
        testRunner.notifyDone();
}

function runTest() {
    document.querySelectorAll('div').forEach(target => target.remove());

    for (let i = 0; i < targetCount; ++i) {
        let target = document.createElement('div');
        target.myState = 'live';
        document.body.appendChild(target);
    }

    document.querySelectorAll('div').forEach(target => observer.observe(target));
}

const observer = new ResizeObserver(entries => {
    for (const entry of entries) {
        if (entry.target.myState != 'live')
            deadCount++;
    }
});

runAll();

</script>
</body>
</html>

```

To fix this, we make targets in ResizeObserver as a GCReachableRef which will add this target to a map. And while GC, the JSElements related to these targets will be reachable.

## ResizeObserverEntry lose JSElement:

```
<!DOCTYPE html><!-- webkit-test-runner [ experimental:ResizeObserverEnabled=true ] -->
<html>
<body>
<p>This tests that JS wrappers of targets in an ResizeObserverEntry do not get collected.</p>
<pre id="log"></pre>
<script src="../resources/gc.js"></script>
<script>

if (window.testRunner)
    testRunner.dumpAsText();

const targetCount = 5;
const iterationCount = 10;

async function observe() {
    for (let i = 0; i < targetCount; ++i) {
        let target = document.createElement('div');
        target.myState = 'live';
        document.body.appendChild(target);
    }

    return new Promise((resolve) => {
        const observer = new ResizeObserver(entries => {
            resolve(entries);
            observer.disconnect();
        });
        document.querySelectorAll('div').forEach(target => observer.observe(target));
    });
}

function check(entries) {
    let deadCount = 0;
    for (const entry of entries) {
        if (entry.target.myState != 'live')
            deadCount++;
    }
    document.getElementById('log').textContent += (deadCount ? `FAIL - ${deadCount} targets lost JS wrappers` : 'PASS') + '\n';
}

async function runAll() {
    if (window.testRunner)
        testRunner.waitUntilDone();

    for (let j = 0; j < iterationCount; ++j) {
        const entries = await observe();
        document.querySelectorAll('div').forEach(target => target.remove());
        await Promise.resolve();
        gc();
        check(entries);
    }

    if (window.testRunner)
        testRunner.notifyDone();
}

runAll();

</script>
</body>
</html>

```

ResizeObserverEntry couldn't use the same resolution (GCReachableRef). Cause if JS sets `entry.target.myEntry = entry`, the target couldn't be released. 

So we will use another way, add JSCustomMarkFunction option to ResizeObserverEntry.idl. Overload visitAdditionalChildren in ResizObserverEntryCustom.cpp and add target to addOpaqueRoot, make it reachable. 

## The basic knowledge about GC.

下面用中文，哇哈哈哈～

- js对象缓存的地方在JSDOMWrapper. 为了保证JSDOMWrapper的唯一性，会把其加入到JSDOMWrapperCache cache中。所以取的时候，先getCachedWrapper，若没有在create一个新的。
  - 上面的GC问题，就是缓存中的JSDOMWrapper被GC了，后面要用的时候，重新创建，导致找不到之前存在JSDOMWrapper的attr了。
- GC的原理大概是，先通过根节点（或者各种map，stack等）mark正在使用的obj，然后释放那些没被mark的obj。
- 存在window里面的obj，如：ResizeObserverEntry.
  - JSDOMWindow::JSDOMGlobalObject => DOMWrappedWorld world => DOMObjectWrapperMap m_wrappers 保存着 key: ResizeObserverEntry value: JSObject(JSResizeObserverEntry, JSResizeObserverEntryOwner, DOMWrappedWorld).
  - world 保存在 JSGlobalObject(其实就是JSDOMWindow)中。 JSDOMWindow是通过JSResizeObserverCallback获取到的，而callback是由JSResizeObserver创建的。
- 那么，JSResizeObserver是如何创建的？
  - 通过llint_op_get_from_scope获取JSResizeObserver的costruct。
  - 然后通过llint_op_construct调用JSResizeObserverContruct::construct创建resizeObserver, then create JSResizeObserver and JSResizeObserverCallback.
- JSDOMWindow每个页面都有一个。
- WeakBlock和JSResizeObserver之间的关系
  - WeakBlock 保存这weakImpls
  - WeakImpls 就是wrapper中保存的value。
  - Weakimpl->WeakHandleOwner就是JSResizeObserverOwner.
  - finalize(WeakImpl())就是调用owner的finialize方法，即在JSDOMWrapperCache中 uncacheWrapper， 下一步就是deallocate， 把这个key-value在wrapper中删除。
- sweep
  - 从weakBlock中过来的
  - weakimpl有个state决定是否删除：live，dead, finalized, deallocated
  - 流程是：先finalized, 然后deallocated,然后把weakimpl加入到freelist中。
  - frerlist代表可用的weakimpl，存在与sweep result。
  - sweep其实就是遍历weakblock下的所有weakimpl(连续存储), finalize dead weakimpl, then put it into freelist.
- WeakSet contains weakblock list.
- markedBlock contains weakset
- weakimpl是什么时候被设置成dead的？
  - WeakBlock::reap()中，遍历每个weakimpl,判断是否marked，若没被marked，则设置成dead。
- 如何判断是否marked？
  - 在markblock中的footer，有个`m_marks`的bitmap，存储mark的状态，通过markedBlock::testandsetmarked可以设置marked信息。
  - weakblock和markedblock通过container来联系： weakblock => cellcontainer => markedblock.
- 大环境
  - heap => markedspace => weakset => weakblock => weakimpl.
  - cellcontainer => markedblock
  - cellcontainer => weakBlock
- cell
  - slotvisitor::setmarkedandappledtomarkstack
  - slotvisitor::drain()从markstack中取得cell
- 如何mark？
  - 通过`cell->visitchildren`, 从cell中取得markedblock,然后mark。
- stack
  - 有两个 `m_collectorstack`, `m_mutatorstack`
- 总之：
  - slotvisitor::drain(): mark
  - weakblock::reap() 通过mark信息改变weakimpl的state：live, dead, finalized, deallocted
  - weakBlock::sweep() deallocator and add to freelist.


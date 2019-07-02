# How to write memory leak test case in WebKit?

I've token a very long time to write the element-leak.html. I think it's worthy to write it down.

## Elements leak

If you suspect that a page has element leak problem, you can put this page in an iframe. And after this iframe loaded or some operation leading to elment leaking has done, remove this iframe and detect if the document of this iframe could be release properly, i.g. `!isDocumentAlive()`.

```
documentIdentifierArray[count++] = internals.documentIdentifier(iframe.contentDocument);

...

internals.isDocumentAlive(frameDocumentIdentifier);

```

## Regarding `promise_test`

About promise, this blog seems easy to understand: https://www.jianshu.com/p/d0551330615d

```
promise_test(async () => {
    return startTest();
}, 'Test elements leak in 20 iframes');
```

`startTest()` should return a promise which could be the promise in startTest or the promise in 

```
return promise.then(function() {
                       return another promise;
                   })

```



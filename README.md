# zdiscgo
[CZMQ](http://github.com/zeromq/czmq) service discovery [zactor](http://czmq.zeromq.org/czmq4-0:zactor) with support for [Go plugins](https://medium.com/learning-the-go-programming-language/calling-go-functions-from-other-languages-4c7d8bcc69bf#.hguiewq19).

## Problem
I want to use various service discovery mechanisms with CZMQ, such as consul and mesos-dns. These APIs do not have goog straight C client libraries but do have excellent Go libraries

## Solution
zdiscgo - a CZMQ actor service that runs as a thread in a C application, and proxies service discovery lookup calls to dynamically loaded and bound Go libraries.

## Why Did You Do This What Is Wrong With You
As Larry Wall said, the first great virtue of a programmer is laziness. If C can talk to Go and Go libraries exist for what I want to do, why not use them.

You might feel compelled to point out this all looks like more trouble than just writing some C client libraries. In retort, I would note that Larry Wall also said the first great virtue of a programmer is hubris, of which this project contains plenty ;)

## Status
Unstable. API will change.

## Writing a Go Plugin

Go plugins should export a DiscoverEndpoints function:

```go
package main

import "C"
import "fmt"

//export DiscoverEndpoints
func DiscoverEndpoints(url, key string) *C.char {
	return C.CString(fmt.Sprintf("inproc://%s-%s", url, key))
}
j
func main() {}
```

## Using the ZActor

```c
zactor_t *zdiscgo = zactor_new (zdiscgo_actor, NULL);
zstr_send (zdiscgo, "VERBOSE");
    
zstr_sendx (zdiscgo, "CONFIGURE", "./go/libmockdiscgo.so", NULL);
zsock_wait (zdiscgo);

zstr_sendx (zdiscgo, "DISCOVER", "url", "key", NULL);
   
char *endpoints = zstr_recv (zdiscgo);
assert (streq (endpoints, "inproc://url-key"));

zactor_destroy (&zdiscgo);

```

## License
MPLv2

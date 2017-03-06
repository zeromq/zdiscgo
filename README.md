# zdiscgo
[CZMQ](http://github.com/zeromq/czmq) service discovery [zactor](http://czmq.zeromq.org/czmq4-0:zactor) with support for [Go plugins](https://medium.com/learning-the-go-programming-language/calling-go-functions-from-other-languages-4c7d8bcc69bf#.hguiewq19).

## Problem
I want to use various service discovery mechanisms with CZMQ, such as consul and mesos-dns. These APIs do not have good straight C client libraries but do have excellent Go libraries

## Solution
zdiscgo - a CZMQ actor service that runs as a thread in a C application, and proxies service discovery lookup calls to dynamically loaded and bound Go libraries.

## Why Did You Do This What Is Wrong With You
As Larry Wall said, the first great virtue of a programmer is laziness. If C can talk to Go and Go libraries exist for what I want to do, why not use them.

You might feel compelled to point out this all looks like more trouble than just writing some C client libraries, and that
in fact I was not being lazy at all. In retort, I would note that Larry Wall also said the third great virtue of a programmer
is hubris, of which this project contains plenty.

## Status
Draft. API will change.

## Writing a Go Plugin

Go plugins should export a DiscoverEndpoints function:

```go
package main

import "C"
import "fmt"

//export ZDiscgoDiscoverEndpoints
func ZDiscgoDiscoverEndpoints(url, key string) *C.char {
	return C.CString(fmt.Sprintf("inproc://%s-%s", url, key))
}

func main() {}
```

## Using zdiscgo

```c
#include <zdiscgo.h>

int main (int argc, char *argv [])
{
    //  Create a zdiscgo instance. This will spin up a new OS level
    //  thread that will handle service discovery requests.

    zactor_t *zdiscgo = zactor_new (zdiscgo_actor, NULL);

    // Set the actor to  verbose mode

    zdiscgo_verbose (zdiscgo);

    // Load a go library
    
    int rc = zdiscgo_load_plugin (zdiscgo, "./go/libmockdiscgo.so");
    assert (rc == 0);

    // Ask for a list of zeromq endpoints

    char *endpoints = zdiscgo_discover (zdiscgo, "url", "key");

    //  Check that we have the correct response

    assert (streq (endpoints, "inproc://url-key"));

    //  Shut down the zdisgco instance and clean up memory.

    zactor_destroy (&zdiscgo);
}
```

## License
[MPLv2](https://github.com/taotetek/zdiscgo)

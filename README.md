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

//export DiscoverEndpoints
func DiscoverEndpoints(url, key string) *C.char {
	return C.CString(fmt.Sprintf("inproc://%s-%s", url, key))
}

func main() {}
```

## Using the ZActor
### Overview
The CZMQ zactor [class](http://czmq.zeromq.org/czmq4-0:zactor) provides a simple actor framework. 
A CZMQ actor is implemented as a thread with a ZMQ_PAIR connection back to it's parent.
The zactor instance acts as a zsock_t and you can pass it to any CZMQ method that accepts
a zsock. Commands are sent to the actor thread in this manner.

### Example
```c
// Create a zdiscgo instance. This will spin up a new OS level
// thread that will handle service discovery requests.

zactor_t *zdiscgo = zactor_new (zdiscgo_actor, NULL);

// We communicate with the service discovery thread over
// a ZMQ_PAIR socket. You can pass the zdisgco instance
// to any CZMQ methods that accept zsock_t *. 
// Let's set the service to verbose mode.

zstr_send (zdiscgo, "VERBOSE");

// Next, let's configure the service by telling it to load 
// our go shared library. The zstr_sendx command will send
// multiple string frames. A NULL terminates the message.
    
zstr_sendx (zdiscgo, "CONFIGURE", "./go/libmockdiscgo.so", NULL);

// Wait for a return signal that lets us know the configure
// has completed.

zsock_wait (zdiscgo);

// Now let's get some endpoints! We send a DISCOVER command
// that consists of the url of a service discovery service,
// and the identifer the service should use to find the 
//  endpoints we want.

zstr_sendx (zdiscgo, "DISCOVER", "http://example.com", "some identifier", NULL);

// zdiscgo will send back the endpoints as a comma delimited
// list compatible with zsock commands.
   
char *endpoints = zstr_recv (zdiscgo);

// Here we make a new ZMQ_SUB socket and pass it our discovered  endpoints.

zsock_t *client = zsock_new_sub (endpoints, "some_topic");

// Shut down the zdisgco instance and clean up memory.

zactor_destroy (&zdiscgo);

```

## License
[MPLv2](https://github.com/taotetek/zdiscgo)

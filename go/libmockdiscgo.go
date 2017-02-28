package main

import "C"

//export DiscoverEndpoints
func DiscoverEndpoints(url, key string) *C.char {
	return C.CString("inproc://iwillnotbemocked")
}

func main() {}

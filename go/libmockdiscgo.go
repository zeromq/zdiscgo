package main

import "C"
import "fmt"

//export DiscoverEndpoints
func DiscoverEndpoints(url, key string) *C.char {
	return C.CString(fmt.Sprintf("inproc://%s-%s", url, key))
}

func main() {}

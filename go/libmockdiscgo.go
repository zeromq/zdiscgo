package main

import "C"
import "fmt"

//export DiscoverEndpoints
func DiscoverEndpoints(url, key string) string {
	return fmt.Sprintf("inproc://%s-%s", url, key)
}

func main() {}

default:
	go build -o libmockdiscgo.so -buildmode=c-shared libmockdiscgo.go

install:
	install ./libmockdiscgo.so /usr/local/lib/libmockdiscgo.so
	install ./libmockdiscgo.h /usr/local/include/libmockdiscgo.h
	install ./libmockdiscgo.pc /usr/local/lib/pkgconfig/libmockdiscgo.pc

clean:
	rm -f libmockdiscgo.so
	rm -f libmockdiscgo.h

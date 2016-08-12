all: b32sort.o libkb32sort.so test

b32sort.o: b32sort.c b32sort.h
	gcc  -O2 -c b32sort.c

test: b32sort_test.c b32sort.c b32sort.h
	gcc  -O2 -o b32sort_test b32sort_test.c b32sort.c

libkb32sort.so: kb32sort.c b32sort.c
	gcc -O2 -m64 -fPIC -shared -I klib/include  -o libkb32sort.so kb32sort.c b32sort.c

clean:
	rm -f *.o b32sort_test libkb32sort.so


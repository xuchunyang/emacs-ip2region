IP2REGION = ip2region/binding/c/

run: a.out
	./a.out

a.out: ip2region.o ip2region-module.o
	cc ip2region.o ip2region-module.o -o $@

ip2region.o: $(IP2REGION)/ip2region.c
	cc -std=c99 -I$(IP2REGION) -c $<

ip2region-module.o: ip2region-module.c
	cc -std=c99 -c $<

clean:
	-rm -vf *.o *.so a.out

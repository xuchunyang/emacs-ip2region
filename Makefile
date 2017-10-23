IP2REGION	= ip2region/binding/c/
EMACS_ROOT	= $(HOME)/src/emacs/
EMACS_SRC	= $(EMACS_ROOT)/src/
EMACS		= $(EMACS_ROOT)/src/emacs

check: ip2region-module.so
	$(EMACS) --batch -Q -L . -l $< -l ip2region --eval '(message "%s" (ip2region "116.62.40.117"))'

ip2region-module.so: ip2region-module.o ip2region.o
	$(CC) -shared ip2region-module.o ip2region.o -o $@

ip2region.o: $(IP2REGION)/ip2region.c
	$(CC) -std=c99 -I$(IP2REGION) -c $<

ip2region-module.o: ip2region-module.c
	$(CC) -std=c99 -Wall -I$(EMACS_SRC) -c $<

clean:
	-rm -vf *.o *.so a.out

prefix ?= /usr
CCFLAGS = -O3 -Wextra -Wall

hydra: hydra.o main.c

test: hydra.o test.c

.PHONY: run, run-test, install, clean

run: hydra
	./hydra hydras/git

run-test: test
	./test

install: hydra
	install -Dm755 hydra $(DESTDIR)$(prefix)/bin/hydra
	install -Dm755 hydra-completion.bash $(DESTDIR)$(prefix)/share/hydra/hydra-completion.bash
	cp --recursive --force hydras $(DESTDIR)$(prefix)/share/hydra/

clean:
	rm -f *.o hydra test

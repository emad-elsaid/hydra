RELEASE_CFLAGS = -O3
DBG_CFLAGS = -g -Wall

hydra: main.c hydra.c
	clang $(RELEASE_CFLAGS) -o hydra main.c

debug/hydra: main.c
	mkdir -p debug
	clang $(DBG_CFLAGS) -o debug/hydra main.c

debug/test: test.c hydra.c
	mkdir -p debug
	clang $(DBG_CFLAGS) -o debug/test test.c

.PHONY: build, debug, run, install, test
build: hydra

debug: debug/hydra

run: debug
	@./debug/hydra hydras/git

test: debug/test
	@./debug/test

install:
	install -Dm755 hydra /usr/bin/hydra
	install -Dm755 hydra-completion.bash /usr/share/hydra/hydra-completion.bash
	cp --recursive --force hydras /usr/share/hydra/

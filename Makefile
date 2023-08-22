RELEASE_CFLAGS = -O3
DBG_CFLAGS = -g -Wall

hydra: main.c
	clang $(RELEASE_CFLAGS) -o hydra main.c

debug/hydra: main.c
	mkdir -p debug
	clang $(DBG_CFLAGS) -o debug/hydra main.c

.PHONY: build, debug, run, install
build: hydra

debug: debug/hydra

run: debug
	@./debug/hydra hydras/git

install:
	install -Dm755 hydra /usr/bin/hydra
	install -Dm755 hydra-completion.bash /usr/share/hydra/hydra-completion.bash
	cp --recursive --force hydras /usr/share/hydra/

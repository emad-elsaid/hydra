build: main.c
	clang -O3 -o hydra main.c

debug: main.c
	@clang -g -Wall -o hydra main.c

run: debug
	@./hydra hydras/git

install:
	install -Dm755 hydra /usr/bin/hydra
	install -Dm755 hydra-completion.bash /usr/share/hydra/hydra-completion.bash
	cp --recursive --force hydras /usr/share/hydra/

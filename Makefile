build:
	clang -O3 -o hydra main.c

debug:
	@clang -g -Wall -o hydra main.c

run: debug
	@./hydra hydras/git

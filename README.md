Hydra
======

a C implementation of [Emacs Hydra](https://github.com/abo-abo/hydra) package to be used in terminal.

# Build

To build from source:

```
clang -o hydra main.c
```

# Hydra file

* Hydra files are simple CSV files that define the groups and commands hydra can load.
* The file has 3 columns without header. an example is under `hydras` directory in this repo.

a simple example can be as follow
```csv
g,Git,
gs,Status,git status
```

* it defines `Git` group under the key `g`
* and defines a command named `Status` under key `s` inside group `g` that will execute the command `git status`

# Running

You can run `hydra` binary with a list of hydra files passed to it for example:

```
hydra hydras/git hydras/systemd hydras/github
```

Hydra will load the files in the order passed.


# Project status

The project is still in the prototype phase. I haven't written C in a while (10+
years) so I'm a bit rusty. probably there could be a lot of improvements.
However I would encourage you to try it out.

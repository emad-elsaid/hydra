𓆚 Hydra
======

a C implementation of [Emacs Hydra](https://github.com/abo-abo/hydra) package to be used in terminal.

## What is 𓆚 Hydra

Just like Emacs 𓆚 Hydra. It's a program that groups your terminal commands and bind them to common key on your keyboard. Commands can also be grouped in a tree like structure each node in the tree has a name and key bound to it.

[Spacemacs](https://www.spacemacs.org/) for example is an emacs configuration centered around 𓆚 Hydra commands, with all user commands grouped as trees bound in a mnemonic way. for example:

- opening a file is bound to `space f f`
- deleting current file is bound to `space f D`
- Git status `space g s`

This repo implements a program that can be invoked in your terminal, it reads CSV files of `key,name,command` lines then shows Terminal UI similar to Emacs 𓆚 Hydra and prints the command to stdout.

Binding 𓆚 Hydra to `space` key in your terminal means pressing `space` will make your terminal behave similar to spacemacs if you executed the output with `eval`. this repo has a bash script `hydra-completion.bash` which achieve this binding.

### Other usages

`hydra` doesn't execute the command by default. the program prints the choosen command to standard output. what to do with this output is up to you. you can for example append it to a file. or open the browser with the output or whatever. just like `fzf`.

This means you can use 𓆚 Hydra as part of your scripts to allow the user to navigate a tree of choices and get the final choice piped to another program, GNU coreutils style.

## Build

To build from source:

```
make
```

## Install

build and install
```
make
sudo make install
```

in your `.bashrc` export environment variable `HYDRA` to has a list of hydras files. 𓆚 Hydra example files are installed under `/usr/share/hydra/hydras`.
The following setup 𓆚 Hydra to load `git` commands

```
export HYDRA="/usr/share/hydra/hydras/git"
source /usr/share/hydra/hydra-completion.bash
```

loading `/usr/share/hydra-completion.bash` will bind `hydra` to `Space` key in your keyboard and will invoke `hydra` when the line is empty.

## 𓆚 Hydra file

* 𓆚 Hydra files are simple CSV files that define the commands 𓆚 hydra can load.
* The file has 3 columns without header. an example is under `hydras` directory in this repo.

a simple example can be as follow
```csv
g,Git,
gs,Status,git status
```

* it defines `Git` command that doesn't execute a command under the key `g`
* and defines a command named `Status` under key `s` inside `g` that will execute the command `git status`

## Running

You can run `hydra` binary with a list of 𓆚 hydra files passed to it for example:

```
hydra hydras/git hydras/systemd hydras/github
```

𓆚 Hydra will load the files in the order passed.

Here is an example running `hydras/git`

[![asciicast](https://asciinema.org/a/603564.svg)](https://asciinema.org/a/603564)


## Project status

The project is still in the prototype phase. I haven't written C in a while (10+
years) so I'm a bit rusty. probably there could be a lot of improvements.
However I would encourage you to try it out.

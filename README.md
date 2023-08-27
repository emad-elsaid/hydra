# 𓆚 Hydra

a C implementation of [Emacs Hydra](https://github.com/abo-abo/hydra) package to be used in terminal.

<p align="center"><img width="256" src="public/logo.png" /></p>

[![asciicast](https://asciinema.org/a/603564.svg)](https://asciinema.org/a/603564)

# What is 𓆚 Hydra?

* An implementation of Emacs 𓆚 Hydra functionality to be used in the terminal
* a terminal program that groups your commands and bind them to common key on your keyboard.
* Commands can also be grouped in a tree like structure each node in the tree has a name and key bound to it.
* It reads CSV files of `key,name,command` lines then shows Terminal UI similar to Emacs 𓆚 Hydra and prints the command to stdout.

[Spacemacs](https://www.spacemacs.org/) for example is an emacs configuration centered around 𓆚 Hydra commands, with all user commands grouped as trees bound in a mnemonic way. for example:

- opening a file is bound to `space f f`
- deleting current file is bound to `space f D`
- Git status `space g s`

Binding 𓆚 Hydra to `space` key in your terminal means pressing `space` will make your terminal behave similar to spacemacs if you executed the output with `eval`. this repo has a bash script `hydra-completion.bash` which achieve this binding.

# Other usages

* `hydra` doesn't execute the command by default.
* The program prints the choosen command to standard output.
* What to do with this output is up to you. you can for example append it to a file. or open the browser with the output or whatever. just like `fzf` and `dmenu`.
* Which means you can use 𓆚 Hydra as part of your scripts to allow the user to navigate a tree of choices and get the final choice piped to another program, GNU coreutils style.

# Install

* Clone it from [github](https://github.com/emad-elsaid/hydra) to your machine
```shell
git clone git@github.com:emad-elsaid/hydra.git
cd hydra
```
* Build
```shell
make
```
* Install
```shell
sudo make install
```

* Export `HYDRA` variable to your `.bashrc`. it should have a list of hydras files. 𓆚 Hydra example files are installed under `/usr/share/hydra/hydras`.
* Loading `/usr/share/hydra-completion.bash` will bind `hydra` to `Space` key in your keyboard and will invoke `hydra` when the line is empty.
* For example to use the `git` 𓆚 Hydra:
```shell
export HYDRA="/usr/share/hydra/hydras/git"
source /usr/share/hydra/hydra-completion.bash
```

# 𓆚 Hydra file

* 𓆚 Hydra files are simple CSV files that define the commands 𓆚 hydra can load.
* The file has 3 columns without header. an example is under `hydras` directory in this repo.

a simple example can be as follow
```csv
g,Git,
gs,Status,git status
```

* It defines `Git` command that doesn't execute a command under the key `g`
* and defines a command named `Status` under key `s` inside `g` that will execute the command `git status`

# Running

* You can run `hydra` binary with a list of 𓆚 hydra files
* 𓆚 Hydra will load the files in the order passed.
* it will display the list of top level commands
* When the program reaches a choosen command that doesn't have children it will print the command to standard output
```shell
hydra hydras/git hydras/systemd hydras/github
```

# Tests

To run tests execute

```shell
make run-test
```

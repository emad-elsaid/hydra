#!/usr/bin/env bash

cmd_complete () {
    make run
    echo -n "${PS1@P}" # print prompt again
}

complete -F cmd_complete -E

bind "Space:complete"

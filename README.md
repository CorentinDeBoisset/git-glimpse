# Fast-git-Prompt

`Fast-git-prompt` is a command-line tool that is aimed at generating a git prompt like the one from [zsh-vcs-prompt](https://github.com/yonchu/zsh-vcs-prompt).

The particularity of this tool is that it is aimed at maximizing the performances using the native C interface for git.

## Requirements

* libgit2-dev
* make
* gcc

## Build

You can build the program by running:

    make all

## Using the CLI

You can configure the icons outputed by this tool with the following arguments:

    usage: fast-git-prompt [<options>]

        -v, --verbose                be verbose
        -h, --help                   show this help
        -a, --ahead-sigil=SIG        overwrite the ahead sigil
        -b, --behind-sigil=SIG       overwrite the behind sigil
        -s, --staged-sigil=SIG       overwrite the staged sigil
        -c, --conflicts-sigil=SIG    overwrite the conflicts sigil
        -u, --unstaged-sigil=SIG     overwrite the unstaged sigil
        -U, --untracked-sigil=SIG    overwrite the untracked sigil
        -S, --stashed-sigil=SIG      overwrite the stashed sigil
        -C, --clean-sigil=SIG        overwrite the clean sigil


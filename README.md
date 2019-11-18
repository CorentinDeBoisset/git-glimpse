# Git Glimpse

`git-glimpse` is a command-line tool that is aimed at generating a git prompt like the one from [zsh-vcs-prompt](https://github.com/yonchu/zsh-vcs-prompt).

The particularity of this tool is that it is aimed at maximizing the performances using the native C interface for git (vs. Python that can slow down a terminal)

## Requirements

* libgit2-dev
* make
* gcc

## Installation

You can build the program by running:

    make all

Then install it with

    sudo make install


## Configure for ZSH

You can add a right prompt in ZSH, by using the following line in your `.zshrc`:

    RPROMPT='$(git-glimpse --zsh-mode)'

The exact path to git-glimpse can be found using `which git-glimpse`


## Using the CLI

You can configure the output of this tool with the following arguments:

    usage: git-glimpse [<options>]

        -h, --help                   show this help
        -a, --ahead-sigil=SIG        overwrite the ahead sigil
        -b, --behind-sigil=SIG       overwrite the behind sigil
        -s, --staged-sigil=SIG       overwrite the staged sigil
        -c, --conflicts-sigil=SIG    overwrite the conflicts sigil
        -u, --unstaged-sigil=SIG     overwrite the unstaged sigil
        -U, --untracked-sigil=SIG    overwrite the untracked sigil
        -S, --stashed-sigil=SIG      overwrite the stashed sigil
        -C, --clean-sigil=SIG        overwrite the clean sigil
            --zsh-mode               print the colors using ZSH formatting

## Uninstalling git-glimpse

If you want to uninstall git-glimpse, run

    sudo make uninstall

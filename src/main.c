#include<stdio.h>
#include<stdlib.h>
#include <getopt.h>
#include <string.h>
#include<git2.h>

struct arguments_t {
    int verbose;
    char *ahead_sigil;
    char *behind_sigil;
    char *staged_sigil;
    char *conflicts_sigil;
    char *unstaged_sigil;
    char *untracked_sigil;
    char *stashed_sigil;
    char *clean_sigil;
} arguments;

static const char *opt_string = ":vha:b:s:c:u:U:S:C:";
static const struct option long_opts[] = {
    { "verbose", no_argument, NULL, 'v' },
    { "help", no_argument, NULL, 'h' },
    { "ahead-sigil", required_argument, NULL, 'a' },
    { "behind-sigil", required_argument, NULL, 'b' },
    { "staged-sigil", required_argument, NULL, 's' },
    { "conflicts-sigil", required_argument, NULL, 'c' },
    { "unstaged-sigil", required_argument, NULL, 'u' },
    { "untracked-sigil", required_argument, NULL, 'U' },
    { "stashed-sigil", required_argument, NULL, 'S' },
    { "clean-sigil", required_argument, NULL, 'C' },
    { 0, 0, 0, 0 }
};

void display_usage(void)
{
    puts(
        "usage: fast-git-prompt [<options>]\n"
    );
    puts("    -v, --verbose                be verbose");
    puts("    -h, --help                   show this help");
    puts("    -a, --ahead-sigil=SIG        overwrite the ahead sigil");
    puts("    -b, --behind-sigil=SIG       overwrite the behind sigil");
    puts("    -s, --staged-sigil=SIG       overwrite the staged sigil");
    puts("    -c, --conflicts-sigil=SIG    overwrite the conflicts sigil");
    puts("    -u, --unstaged-sigil=SIG     overwrite the unstaged sigil");
    puts("    -U, --untracked-sigil=SIG    overwrite the untracked sigil");
    puts("    -S, --stashed-sigil=SIG      overwrite the stashed sigil");
    puts("    -C, --clean-sigil=SIG        overwrite the clean sigil\n");
    exit(EXIT_FAILURE);
}

void parse_arguments(int argc, char **argv)
{
    int opt = 0;
    arguments.verbose = 0;

    while (-1 != (opt = getopt_long(argc, argv, opt_string, long_opts, NULL)))
    {
        switch(opt) {
            case 'v':
                arguments.verbose++;
                break;
            case 'h':
                display_usage();
                break;
            case 'a':
                arguments.ahead_sigil = optarg;
                break;
            case 'b':
                arguments.behind_sigil = optarg;
                break;
            case 's':
                arguments.staged_sigil = optarg;
                break;
            case 'c':
                arguments.conflicts_sigil = optarg;
                break;
            case 'u':
                arguments.unstaged_sigil = optarg;
                break;
            case 'U':
                arguments.untracked_sigil = optarg;
                break;
            case 'S':
                arguments.stashed_sigil = optarg;
                break;
            case 'C':
                arguments.clean_sigil = optarg;
                break;
            case ':':
                fprintf(stderr, "option `-%c' requires an argument\n\n", optopt);
                exit(EXIT_FAILURE);
                break;
            case 0:
                break;
            default:
                fprintf(stderr, "option `-%c' is invalid: ignored\n\n", optopt);
                exit(EXIT_FAILURE);
                break;
        }
    }

    if (NULL == arguments.ahead_sigil)
        arguments.ahead_sigil = "↑";
    if (NULL == arguments.behind_sigil)
        arguments.behind_sigil = "↓";
    if (NULL == arguments.staged_sigil)
        arguments.staged_sigil = "●";
    if (NULL == arguments.conflicts_sigil)
        arguments.conflicts_sigil = "✖";
    if (NULL == arguments.unstaged_sigil)
        arguments.unstaged_sigil = "✚";
    if (NULL == arguments.untracked_sigil)
        arguments.untracked_sigil = "…";
    if (NULL == arguments.stashed_sigil)
        arguments.stashed_sigil = "⚑";
    if (NULL == arguments.clean_sigil)
        arguments.clean_sigil = "✔";
}

int main(int argc, char **argv)
{
    parse_arguments(argc, argv);

    printf(
        "%s %s %s %s %s %s %s %s\n",
        arguments.ahead_sigil,
        arguments.behind_sigil,
        arguments.staged_sigil,
        arguments.conflicts_sigil,
        arguments.unstaged_sigil,
        arguments.untracked_sigil,
        arguments.stashed_sigil,
        arguments.clean_sigil
    );
    /* git_libgit2_init(); */

    /* git_libgit2_shutdown(); */

    return 0;
}

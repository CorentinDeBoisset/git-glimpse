#include <stdio.h>
#include <getopt.h>
#include <git2.h>
#include <stdbool.h>
#include <string.h>


#include "functions.h"

#define COLORIZE(color_code, text) "\033["color_code"m"text"\033[39m"

struct options_t {
    char *sigil_ahead;
    char *sigil_behind;
    char *sigil_staged;
    char *sigil_conflicts;
    char *sigil_unstaged;
    char *sigil_untracked;
    char *sigil_stashed;
    char *sigil_clean;
    char *format_string;
} options;

static const char *opt_string = ":vha:b:s:c:u:U:S:C:";
static const struct option long_opts[] = {
    { "help", no_argument, NULL, 'h' },
    { "ahead-sigil", required_argument, NULL, 'a' },
    { "behind-sigil", required_argument, NULL, 'b' },
    { "staged-sigil", required_argument, NULL, 's' },
    { "conflicts-sigil", required_argument, NULL, 'c' },
    { "unstaged-sigil", required_argument, NULL, 'u' },
    { "untracked-sigil", required_argument, NULL, 'U' },
    { "stashed-sigil", required_argument, NULL, 'S' },
    { "clean-sigil", required_argument, NULL, 'C' },
    { "zsh-mode", no_argument, NULL, 0 },
    { 0, 0, 0, 0 }
};

void display_usage(void)
{
    puts(
        "usage: " BINNAME " [<options>]\n"
    );
    puts("    -h, --help                   show this help");
    puts("    -a, --ahead-sigil=SIG        overwrite the ahead sigil");
    puts("    -b, --behind-sigil=SIG       overwrite the behind sigil");
    puts("    -s, --staged-sigil=SIG       overwrite the staged sigil");
    puts("    -c, --conflicts-sigil=SIG    overwrite the conflicts sigil");
    puts("    -u, --unstaged-sigil=SIG     overwrite the unstaged sigil");
    puts("    -U, --untracked-sigil=SIG    overwrite the untracked sigil");
    puts("    -S, --stashed-sigil=SIG      overwrite the stashed sigil");
    puts("    -C, --clean-sigil=SIG        overwrite the clean sigil");
    puts("        --zsh-mode               print the colors using ZSH formatting\n");
    exit(EXIT_FAILURE);
}

void parse_arguments(int argc, char **argv)
{
    int opt = 0;
    int option_index = 0;

    while (-1 != (opt = getopt_long(argc, argv, opt_string, long_opts, &option_index)))
    {
        switch(opt) {
            case 'h':
                display_usage();
                break;
            case 'a':
                options.sigil_ahead = optarg;
                break;
            case 'b':
                options.sigil_behind = optarg;
                break;
            case 's':
                options.sigil_staged = optarg;
                break;
            case 'c':
                options.sigil_conflicts = optarg;
                break;
            case 'u':
                options.sigil_unstaged = optarg;
                break;
            case 'U':
                options.sigil_untracked = optarg;
                break;
            case 'S':
                options.sigil_stashed = optarg;
                break;
            case 'C':
                options.sigil_clean = optarg;
                break;
            case 0:
                if (strcmp(long_opts[option_index].name, "zsh-mode") == 0) {
                    options.format_string = "%%F{white}(git:%s%s%s%%B%%F{red}%s%%b%%f|%%F{cyan}%s%%F{blue}%s%%F{yellow}%s%%F{white}%s%%F{red}%s%%F{green}%s%%f)";
                }
                break;
            case ':':
                fprintf(stderr, "option `-%c' requires an argument\n\n", optopt);
                exit(EXIT_FAILURE);
                break;
            default:
                fprintf(stderr, "option is invalid: ignored\n\n");
                exit(EXIT_FAILURE);
                break;
        }
    }

    if (NULL == options.sigil_ahead)
        options.sigil_ahead = "↑";
    if (NULL == options.sigil_behind)
        options.sigil_behind = "↓";
    if (NULL == options.sigil_staged)
        options.sigil_staged = "●";
    if (NULL == options.sigil_conflicts)
        options.sigil_conflicts = "✖";
    if (NULL == options.sigil_unstaged)
        options.sigil_unstaged = "✚";
    if (NULL == options.sigil_untracked)
        options.sigil_untracked = "…";
    if (NULL == options.sigil_stashed)
        options.sigil_stashed = "⚑";
    if (NULL == options.sigil_clean)
        options.sigil_clean = "✔";
    if (NULL == options.format_string)
        options.format_string = "\033[37m(git:%s%s%s\033[31;1m%s\033[22;39m|\033[22;36m%s\033[34m%s\033[33m%s\033[37m%s\033[31m%s\033[32m%s\033[39m)";
}


int main(int argc, char **argv)
{
    char cwd[BUFSIZ];
    char directory[BUFSIZ];
    git_repository *repo = NULL;
    struct tree_status tstatus;
    struct branch_status bstatus;
    int stash_count;
    int i;

    parse_arguments(argc, argv);
    getcwd(cwd, BUFSIZ);

    git_libgit2_init();

    if ('/' == cwd[0]) {
        for (i = strlen(cwd); i > 0; i--) {
            if (cwd[i] == '/' || cwd[i] == '\0') {
                strncpy(directory, cwd, i);
                directory[i] = '\0';
                if (0 == git_repository_open(&repo, directory)) {
                    stash_count = get_stash_count(repo);
                    get_branch_status(&bstatus, repo);
                    get_tree_status(&tstatus, repo);
                    printf(
                           options.format_string,
                           bstatus.head_name ? bstatus.head_name : "",
                           bstatus.ahead_count ? options.sigil_ahead : "",
                           bstatus.behind_count ? options.sigil_behind : "",
                           bstatus.current_operation ? bstatus.current_operation : "",
                           stash_count ? options.sigil_stashed : "",
                           tstatus.staged_count ? options.sigil_staged : "",
                           tstatus.unstaged_count ? options.sigil_unstaged : "",
                           tstatus.untracked_count ? options.sigil_untracked : "",
                           tstatus.conflict_count ? options.sigil_conflicts : "",
                           (!tstatus.staged_count && !tstatus.unstaged_count && !tstatus.untracked_count && !tstatus.conflict_count) ? options.sigil_clean : ""
                    );

                    if (NULL != bstatus.head_name)
                        free(bstatus.head_name);
                    git_repository_free(repo);

                    break;
                }
            }
        }
    }

    git_libgit2_shutdown();

    return 0;
}
